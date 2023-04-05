public class instr {
    String label; //name of instruction
    int addr; //address of instruction
    int op; //opcode
    boolean isInstr; //checks to see if this is a label
    int[] args; //all arguments passed to instruction
    String type;

    public instr(String data, int addr) {
        /* label */
        this.label = data;
        this.addr = addr;
        this.op = -1;
        this.isInstr = false;
        this.args = null;
        this.type = "None";
    }

    public instr(String data, int addr, int op, int rs, int rt, int rd, int shamt, int funct) {
        /* R type instruction*/
        this.label = data;
        this.addr = addr;
        this.op = op;
        this.isInstr = true;
        args = new int[5];
        args[0] = rs;
        args[1] = rt;
        args[2] = rd;
        args[3] = shamt;
        args[4] = funct;
        this.type = "R";
    }

    public instr(String data, int addr, int op, int rs, int rt, int imm) {
        /* I type instruction*/
        this.label = data;
        this.addr = addr;
        this.op = op;
        this.isInstr = true;
        args = new int[3];
        args[0] = rs;
        args[1] = rt;
        args[2] = imm;
        this.type = "I";
    }

    public instr(String data, int addr, int op, int argAddr) {
        /* J type instruction*/
        this.label = data;
        this.addr = addr;
        this.op = op;
        this.isInstr = true;
        args = new int[1];
        args[0] = argAddr;
        this.type = "J";
    }
}
