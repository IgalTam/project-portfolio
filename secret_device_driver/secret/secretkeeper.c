#include <minix/drivers.h>
#include <minix/driver.h>
#include <stdio.h>
#include <stdlib.h>
#include <minix/ds.h>
#include <sys/ioc_secret.h>
#include "secretkeeper.h"
#include <sys/ucred.h>
#include <unistd.h>
/*#include <sys/socket.h> */

/*
 * Function prototypes for the secretkeeper driver.
 */
FORWARD _PROTOTYPE( char * secretkeeper_name,   (void) );
FORWARD _PROTOTYPE( int secretkeeper_open,     (struct driver *d, message *m));
FORWARD _PROTOTYPE( int secretkeeper_close,    (struct driver *d, message *m));
FORWARD _PROTOTYPE( struct device * secretkeeper_prepare, (int device) );
FORWARD _PROTOTYPE( int secretkeeper_transfer,  (int procnr, int opcode,
                                          u64_t position, iovec_t *iov,
                                          unsigned nr_req) );
FORWARD _PROTOTYPE( void secretkeeper_geometry, (struct partition *entry) );

FORWARD _PROTOTYPE( int ioctl_handler, (struct driver *d, message *m) );  


/* SEF functions and variables. */
FORWARD _PROTOTYPE( void sef_local_startup, (void) );
FORWARD _PROTOTYPE( int sef_cb_init, (int type, sef_init_info_t *info) );
FORWARD _PROTOTYPE( int sef_cb_lu_state_save, (int) );
FORWARD _PROTOTYPE( int lu_state_restore, (void) );

/* Entry points to the secretkeeper driver. */
PRIVATE struct driver secretkeeper_tab =
{
    secretkeeper_name,
    secretkeeper_open,
    secretkeeper_close,
    ioctl_handler,
    secretkeeper_prepare,
    secretkeeper_transfer,
    nop_cleanup,
    secretkeeper_geometry,
    nop_alarm,
    nop_cancel,
    nop_select,
    nop_ioctl,
    do_nop,
};

/** Represents the /dev/secretkeeper device. */
PRIVATE struct device secretkeeper_device;

/* variable to hold the uid of owner of the device */
PRIVATE struct ucred own_hldr;

/* read file descriptor count */
PRIVATE int fd_count;

/* read happened flag*/
PRIVATE int read_flg;

/* buffer for storing secrets */
PRIVATE char secret_buf[SECRET_SIZE];

PRIVATE int buf_amt; /* amount written to secret_buf */
PRIVATE int buf_rd; /* amount read from secret_buf */

PRIVATE char * secretkeeper_name(void)
{
    printf("secretkeeper_name()\n");
    return "secretkeeper";
}

PRIVATE int secretkeeper_open(d, m)
    struct driver *d;
    message *m;
{
    struct ucred read_metad;

    if(m->COUNT & W_BIT && m->COUNT & R_BIT) {
        /* prevent read-write access */
        errno = EACCES;
        return errno;
    } else if(m->COUNT & W_BIT) {
        /* write evaluation */
        if(buf_amt == SECRET_SIZE) {
            /* can't open full secret for writing */
            errno = ENOSPC;
            return errno;
        }
        else if(own_hldr.uid) {
            /*prevent write access if there is already an owner of the device*/
            errno = EACCES;
            return errno;
        }
        /* otherwise, populate device owner field */
        if(-1 == getnucred(m->IO_ENDPT, &own_hldr)) {
            return errno;
        }
        fd_count++;
    } else if (m->COUNT & R_BIT) {
        /* read evaluation */

        /* get reading process's uid */
        if(-1 == getnucred(m->IO_ENDPT, &read_metad)) {
            return errno;
        } if(own_hldr.uid && read_metad.uid != own_hldr.uid) {
            /* can't read device belonging to another user */
            errno = EACCES;
            return errno;
        }
        read_flg = 1;
        fd_count++;
    }

    return OK;
}

void secretkeeper_reset() {
    /* resets the secretkeeper (used when last read fd closes)*/
    own_hldr.uid = 0;
    fd_count = 0;
    read_flg = 0;
    memset(secret_buf, '\0', SECRET_SIZE);
    buf_amt = 0;
    buf_rd = 0;
}

PRIVATE int secretkeeper_close(d, m)
    struct driver *d;
    message *m;
{
    /* decrement counter of open fds, if any are open */
    if(fd_count > 0) {
        fd_count--;

        /* if no fds are left open and a read occurred, 
        reset the secretkeeper */
        if(!fd_count && read_flg) {
            secretkeeper_reset();
        }
    }

    return OK;
}

PRIVATE int ioctl_handler(struct driver *d, message *m) {
    /* handles ioctl calls for SSGRANT*/

    int res;

    if(m->REQUEST != SSGRANT) {
        /* don't handle non-SSGRANT requests */
        errno = ENOTTY;
        return errno;
    }

    /* assign new uid */
    res = sys_safecopyfrom(m->IO_ENDPT, (vir_bytes)m->IO_GRANT, 
            0, (vir_bytes)&(own_hldr.uid), sizeof(own_hldr.uid), D);
    
    return res; /* also returns any error code from sys_safecopyfrom */
}

PRIVATE struct device * secretkeeper_prepare(dev)
    int dev;
{
    secretkeeper_device.dv_base.lo = 0;
    secretkeeper_device.dv_base.hi = 0;
    secretkeeper_device.dv_size.hi = 0;
    return &secretkeeper_device;
}

PRIVATE int secretkeeper_transfer(proc_nr, opcode, position, iov, nr_req)
    int proc_nr;
    int opcode;
    u64_t position;
    iovec_t *iov;
    unsigned nr_req;
{
    int bytes, ret;

    /* don't bother reading/writing if requested size is <= 0*/
    if(iov->iov_size <= 0)
    {
        return OK;
    }

    switch (opcode)
    {
        case DEV_GATHER_S:
            /* read bytes from secret buffer */

            /* limit read to length of what is written in buffer */
	    if(buf_rd >= buf_amt) {
                bytes = 0;
            } else if(iov->iov_size + buf_rd > buf_amt) {
                bytes = buf_amt - buf_rd;
            } else {
                bytes = iov->iov_size;
            }

	    if(bytes <= 0) {
	    	return OK;
	    }

            ret = sys_safecopyto(proc_nr, iov->iov_addr, 0,
                                (vir_bytes) (secret_buf + buf_rd),
                                 bytes, D);
            iov->iov_size -= bytes;
	        buf_rd += bytes;    /* keep track of last read amount */
            break;

        case DEV_SCATTER_S:
            /* write bytes to secret buffer */
        
            /* if buffer is at capacity, return ENOSPC*/
            if(buf_amt == SECRET_SIZE) {
                errno = ENOSPC;
                return errno;
            }

            /* limit write to buffer size */
            if(iov->iov_size + buf_amt > SECRET_SIZE) {
                bytes = SECRET_SIZE - buf_amt;
            } else {
                bytes = iov->iov_size;
            }

            ret = sys_safecopyfrom(proc_nr, iov->iov_addr, 0,
                                (vir_bytes) (secret_buf + buf_amt),
                                 bytes, D);
            iov->iov_size -= bytes;
	        buf_amt += bytes;    /* keep track of last write amount */
            break;

        default:
            return EINVAL;
    }
    return ret;
}

PRIVATE void secretkeeper_geometry(entry)
    struct partition *entry;
{
    printf("secretkeeper_geometry()\n");
    entry->cylinders = 0;
    entry->heads     = 0;
    entry->sectors   = 0;
}

PRIVATE int sef_cb_lu_state_save(int state) {
/* Save the state. */
    ds_publish_mem("own_hldr", &own_hldr.uid, sizeof(uid_t),
    	DSF_OVERWRITE);
    ds_publish_u32("fd_count", fd_count, DSF_OVERWRITE);
    ds_publish_u32("read_flg", read_flg, DSF_OVERWRITE);
    ds_publish_mem("secret_buf", secret_buf, sizeof(char)*SECRET_SIZE,
        DSF_OVERWRITE);
    ds_publish_u32("buf_amt", buf_amt, DSF_OVERWRITE);
    ds_publish_u32("buf_rd", buf_rd, DSF_OVERWRITE);

    return OK;
}

PRIVATE int lu_state_restore() {
/* Restore the state. */
    u32_t value;
    char val_buf[SECRET_SIZE];
    size_t buf_size, uid_size;

    /* owner uid info*/
    uid_size = sizeof(uid_t); 
    ds_retrieve_mem("own_hldr", val_buf, &uid_size);
    ds_delete_mem("own_hldr");
    own_hldr.uid = (uid_t) *val_buf;

    /* number of opens for reading*/
    ds_retrieve_u32("fd_count", &value);
    ds_delete_u32("fd_count");
    fd_count = (int) value;

    /* number of opens for reading*/
    ds_retrieve_u32("read_flg", &value);
    ds_delete_u32("read_flg");
    read_flg = (int) value;

    /* secretkeeper buffer*/
    buf_size = sizeof(char)*SECRET_SIZE; 
    ds_retrieve_mem("secret_buf", val_buf, &buf_size);
    ds_delete_mem("secret_buf");
    memcpy(secret_buf, val_buf, SECRET_SIZE);

    /* buffer written amount*/
    ds_retrieve_u32("buf_amt", &value);
    ds_delete_u32("buf_amt");
    buf_amt = (int) value;

    /* buffer read amount*/
    ds_retrieve_u32("buf_rd", &value);
    ds_delete_u32("buf_rd");
    buf_rd = (int) value;
    
    return OK;
}

PRIVATE void sef_local_startup()
{
    /*
     * Register init callbacks. Use the same function for all event types
     */
    sef_setcb_init_fresh(sef_cb_init);
    sef_setcb_init_lu(sef_cb_init);
    sef_setcb_init_restart(sef_cb_init);

    /*
     * Register live update callbacks.
     */
    /* - Agree to update immediately when LU is requested in a valid state. */
    sef_setcb_lu_prepare(sef_cb_lu_prepare_always_ready);
    /* - Support live update starting from any standard state. */
    sef_setcb_lu_state_isvalid(sef_cb_lu_state_isvalid_standard);
    /* - Register a custom routine to save the state. */
    sef_setcb_lu_state_save(sef_cb_lu_state_save);

    /* Let SEF perform startup. */
    sef_startup();
}

PRIVATE int sef_cb_init(int type, sef_init_info_t *info)
{
/* Initialize the secretkeeper driver. */
    int do_announce_driver = TRUE;

    /* initialize state data*/
    secretkeeper_reset();

    switch(type) {
        case SEF_INIT_FRESH:
            ;
        break;

        case SEF_INIT_LU:
            /* Restore the state. */
            lu_state_restore();
            do_announce_driver = FALSE;

            printf("Hey, I'm a new version!\n");
        break;

        case SEF_INIT_RESTART:
            printf("Hey, I've just been restarted!\n");
        break;
    }

    /* Announce we are up when necessary. */
    if (do_announce_driver) {
        driver_announce();
    }

    /* Initialization completed successfully. */
    return OK;
}

PUBLIC int main(int argc, char **argv)
{
    /*
     * Perform initialization.
     */
    sef_local_startup();

    /*
     * Run the main loop.
     */
    driver_task(&secretkeeper_tab, DRIVER_STD);
    return OK;
}

