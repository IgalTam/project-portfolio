public class instrHt {
    instr[] ht;
    int totalElem;
    int capacity;
    int finAddr; /* address of last instruction in file*/

    public instrHt(int capacity) {
        ht = new instr[capacity];
        totalElem = 0;
        this.capacity = capacity;
        finAddr = 0;
    }

    private int hashFunc(instr it) {
        //creates a hash based on the label of it
        int hashed = 0;
        for(String c: it.label.split("")) {
            hashed = (hashed * 31 + c.charAt(0)) % this.capacity;
        }
        return hashed;
    }

    private float loadFactor() {
        //returns load factor of hashtable
        return (float) this.totalElem / this.capacity;
    }

    public void insert(instr it) {
        //inserts instruction into hashtable with linear probing
        int hash = hashFunc(it);
        boolean fin = false;
        while(!fin) {
            if (this.ht[hash] == null) {
                this.ht[hash] = it;
                this.totalElem++;
                fin = true;
            } else {
                hash = (hash + 1) % this.capacity;
            }
        }
        //refactor table if needed
        if( loadFactor() > 0.75) {
            this.capacity = 2 * this.capacity + 1;
            instr[] oldHt = this.ht;
            this.ht = new instr[capacity];
            this.totalElem = 0;
            for (instr i : oldHt) {
                if(i != null)
                    insert(i);
            }
        }
    }

    public instr getByAddr(int addr) {
        //retrieves an instruction from hash table by its address
        for(int i = 0; i < this.capacity; i++) {
           if(ht[i] != null && ht[i].isInstr && ht[i].addr == addr) {
               return ht[i];
           }
        }
        return null;
    }

    public int getLabelAddrByName(String label) {
        //retrieves a label address from hash table
        for(int i = 0; i < this.capacity; i++) {
            if(ht[i] != null && !ht[i].isInstr && ht[i].label.equals(label)) {
                return ht[i].addr;
            }
        }
        return -1;
    }

}
