/* Name: Igal Tamarkin, Ryan Zhang
 *  Section: 07/08
 *  Lab 4: CPU Simulator
 */

import java.io.File;
import java.io.FileNotFoundException;
import java.util.Arrays;
import java.util.Scanner;
import java.util.List;
import java.util.ArrayList;


public class lab4 {

    private int pc;
    private int instrCount;
    private int cycles;
    private int[] dm;
    private int[] rf;
    private instr[] pipeline;
    private int branch_taken;

    public lab4(instr[] instrArr) {
        this.pc = 0;
        this.instrCount = 0;
        this.cycles = 0;
        this.dm = new int[8192];
        this.rf = new int[32];
        this.pipeline = new instr[5];
        Arrays.fill(this.pipeline, new instr("empty", -1));
        this.branch_taken = -100;
    }

    public void scriptMode(instr[] instrArr, String scriptFile) throws FileNotFoundException {
        /* executes given input script file and uses operations on input asm file*/
        File sf = new File(scriptFile);
        Scanner rdr = new Scanner(sf);
        String input;
        /* display prompt*/
        System.out.print("\nmips> ");
        while( !(input = rdr.nextLine()).equals("q")) {
            System.out.println(input + "\n");
            String[] dataArr = parseCmd(input);
            if(dataArr != null)
                execCmd(dataArr, instrArr);
            System.out.print("mips> ");
        }
        System.out.println(input);
        rdr.close();
    }

    public void shellMode(instr[] instrArr) {
        /* enters interactive mode to perform operations on input asm file*/
        Scanner rdr = new Scanner(System.in);
        String input;
        /* display prompt*/
        System.out.print("\nmips> ");
        while( !(input = rdr.nextLine()).equals("q") ) {
            System.out.println();
            String[] dataArr = parseCmd(input);
            if (dataArr != null)
                execCmd(dataArr, instrArr);
            System.out.print("mips> ");
        }
        rdr.close();
    }

    private String[] parseCmd(String stdin) {
        /* parses the given input and returns it in an array*/
        String[] stdinArr = stdin.split(" ");
        int errCode = 0;
        switch (stdinArr[0]) {
            case "h":
            case "d":
            case "r":
            case "c":
                if (stdinArr.length > 1)
                    errCode = 1;
                break;
            case "s":
                if (stdinArr.length > 2)
                    errCode = 1;
                else if(stdinArr.length == 2) {
                    try {
                        Integer.parseInt(stdinArr[1]);
                    } catch(NumberFormatException e) {
                        errCode = 2;
                        break;
                    }
                }
                break;
            case "m":
                if (stdinArr.length != 3) {
                    errCode = 1;
                    break;
                } try {
                Integer.parseInt(stdinArr[1]);
                Integer.parseInt(stdinArr[2]);
            } catch(NumberFormatException e) {
                errCode = 2;
                break;
            }
                break;
            default:
                System.out.println("\t" + stdinArr[0] + ": Invalid command\n");
                return null;
        }
        if(errCode == 0)
            return stdinArr;
        else if(errCode == 1) {
            System.out.println("\t" + stdinArr[0] + ": invalid amount of arguments\n");
            return null;
        } else if(errCode == 2) {
            System.out.println("\t" + stdinArr[0] + ": invalid argument type\n");
            return null;
        }
        System.out.println("\t something unknown went wrong\n");
        return null;
    }

    private void execCmd(String[] stdin, instr[] instrArr) {
        /* executes the given command*/
        int i, stepAmt, val, val_exec=0;
        switch (stdin[0]) {
            case "h": System.out.println("h = show help");
                System.out.println("d = dump register state");
                System.out.println("p = show pipeline registers");
                System.out.println("s = step through a single clock cycle step (i.e. simulate 1 cycle and stop)");
                System.out.println("s num = step through num clock cycles");
                System.out.println("r = run until the program ends and display timing summary");
                System.out.println("m num1 num2 = display data memory from location num1 to num2");
                System.out.println("c = clear all registers, memory, and the program counter to 0");
                System.out.println("q = exit the program\n");
                break;
            case "d": dumpRegs();
                break;
            case "p": displayPipeline();
                break;
            case "r": while(pc < instrArr.length) {
                    val = adjustPipeline();
                    //System.out.println(this.pipeline[4].label);
                    if(this.branch_taken == this.pipeline[4].addr)
                        controlHazard();
                    else {
                        if(1 == (val_exec = execInstr(this.pipeline[2])))
                            this.branch_taken = this.pipeline[2].addr;
                        if(val == 0)
                            this.pipeline[1] = instrArr[pc];
                        /*if(pc == 4)
                            System.out.println("at end, instr is: " + instrArr[pc].label +
                                    ", end address is: " + instrArr[pc].args[2] + ", pc = 0 has instr: " +
                                    instrArr[0].label);*/
                    }
                    //System.out.println("val: " + val + ", branch_taken: " + this.branch_taken);
                        /*for(i = 0; i < 5; i++) {
                            if(this.pipeline[i] != null)
                                System.out.println(this.pipeline[i].label);
                            else
                                System.out.println(this.pipeline[i]);
                        }*/
                    if(0 == val && (0 == val_exec || 4 == val_exec))
                        pc++;
                    if(0 == val && !this.pipeline[2].label.equals("squash"))
                        this.instrCount++;
                    this.cycles++;
                    //displayPipeline();
                }
                while(!this.pipeline[4].label.equals("empty")) {
                    adjustPipeline();
                    this.cycles++;
                }
                displayEndStats();
                System.out.println("\n");
                break;
            case "c": Arrays.fill(this.dm, 0);
                Arrays.fill(this.rf, 0);
                this.pc = this.instrCount = this.cycles = 0;
                System.out.println("\tSimulator reset\n");
                break;
            case "s": if(stdin.length == 1)
                    stepAmt = 1;
                else
                    stepAmt = Integer.parseInt(stdin[1]);
                while(stepAmt > 0 && pc < instrArr.length) {
                    val = adjustPipeline();
                    if(this.branch_taken == this.pipeline[4].addr)
                        controlHazard();
                    else {
                        if(1 == (val_exec = execInstr(this.pipeline[2])))
                            this.branch_taken = this.pipeline[2].addr;
                        if(val == 0)
                            this.pipeline[1] = instrArr[pc];
                    }
                    if(0 == val && (0 == val_exec || 4 == val_exec))
                        pc++;
                    if(0 == val && !this.pipeline[2].label.equals("squash"))
                        this.instrCount++;
                    this.cycles++;
                    displayPipeline();
                    stepAmt--;
                    if(pc > instrArr.length)
                        break;
                } if(pc >= instrArr.length)
                    System.out.println("\tEnd of program, reset simulator or run to continue");
                break;
            case "m": for(i = Integer.parseInt(stdin[1]); i < Integer.parseInt(stdin[2])+1; i++)
                    System.out.println(String.format("[%d]: %d", i, this.dm[i]));
                System.out.println("\n");
                break;
            default: System.out.println("Invalid Command");
                System.out.println("\n");
                break;
        }
    }

    private void displayPipeline() {
        /* displays all instructions in each pipeline stage */
        System.out.println("pc\tif/id\tid/exe\texe/mem\tmem/wb");
        System.out.print(pc);
        for(int i = 1; i < 5; i++)
            System.out.print("\t" + this.pipeline[i].label);
        System.out.println("\n");
    }

    private void displayEndStats() {
        /* displays cpi, cycles, and instructions */
        double cpi = (double) this.cycles/this.instrCount;
        System.out.println("Program complete");
        System.out.println(String.format("CPI = %.3f\tCycles = %d\tInstructions = %d",
                cpi, this.cycles, this.instrCount));
    }

    private int adjustPipeline() {
        /* adjustPipeline version 2*/
        instr[] pipelineNew = new instr[5];
        /*if(this.pipeline[2].args != null && this.pipeline[1].args != null) {
            System.out.println("lw dest " + this.pipeline[2].args[1] + " ual rs " +
                    this.pipeline[1].args[0] + " ual rt " + this.pipeline[1].args[1]);
        }*/
        if(this.pipeline[2].label.equals("lw")
                && !this.pipeline[1].type.equals("J")
                && this.pipeline[2].args[1] != 0
                && ( (this.pipeline[1].type.equals("R") && (this.pipeline[2].args[1] == this.pipeline[1].args[0]
                || this.pipeline[2].args[1]  == this.pipeline[1].args[1])) || (this.pipeline[1].type.equals("I")
                && this.pipeline[2].args[1] == this.pipeline[1].args[0]))) {
            /* UAL: insert stall into EX stage after shifting EX, MEM, and WB stages */
            System.arraycopy(this.pipeline, 2, pipelineNew, 3, 2);
            pipelineNew[2] = installStall(this.pipeline[1].addr);
            System.arraycopy(this.pipeline, 0, pipelineNew, 0, 2);
            /*for(int i = 0; i < 5; i++) {
                if(pipelineNew[i] != null)
                    System.out.println(pipelineNew[i].label);
                else
                    System.out.println(pipelineNew[i]);
            }*/
            this.pipeline = pipelineNew;
            return 1;
        } else if(this.pipeline[1].type.equals("J")) { /*jump control hazard*/
            System.arraycopy(this.pipeline, 1, pipelineNew, 2, 3);
            pipelineNew[1] = installSquash(this.pipeline[1].addr);
            pipelineNew[0] = new instr("empty", -1);
            this.pipeline = pipelineNew;
            return 2;
        } else  { /* non-hazard procedure*/
            System.arraycopy(this.pipeline, 0, pipelineNew, 1, 4);
            pipelineNew[0] = new instr("empty", -1);
            this.pipeline = pipelineNew;
            return 0;
        }
    }

    private void controlHazard() {
        /* control hazard handling: if a branch is taken,
        * squash the three following instructions when it the
        * branch instr reaches mem/wb register */
        //for(int i = 0; i < this.pipeline[4].args.length; i++)
            //System.out.println(this.pipeline[4].args[i]);
        instr[] pipelineNew = new instr[5];
        /*for(int i = 0; i < 5; i++) {
            if(this.pipeline[i] != null)
                System.out.println(this.pipeline[i].label);
            else
                System.out.println(this.pipeline[i]);
        }*/
        System.arraycopy(this.pipeline, 4, pipelineNew, 4, 1);
        for(int i = 1; i < 4; i++)
            pipelineNew[i] = installSquash(this.pipeline[i].addr);
        pipelineNew[0] = new instr("empty", -1);
        this.pipeline = pipelineNew;
        this.branch_taken = 0;
        /*System.out.println("after shift");
        for(int i = 0; i < 5; i++) {
            if(this.pipeline[i] != null)
                System.out.println(this.pipeline[i].label);
            else
                System.out.println(this.pipeline[i]);
        }*/
        pc += this.pipeline[4].args[2] - 3;
    }

    private instr installStall(int addr) {
        /* in event of a data hazard, create a cycle delay
        * in the form of an instr object named "stall", an NOP */
        return new instr("stall", addr);
    }

    private instr installSquash(int addr) {
        /* in event of a control hazard, create a cycle delay
         * in the form of an instr object named "squash", an NOP */
        return new instr("squash", addr);
    }

    private void dumpRegs() {
        /* displays all register data in stdout*/
        int indCount = 0;
        System.out.println(String.format("pc = %d", this.pc));
        for(int i = 0; i < 32; i++) {
            if (i != 1 && i != 26 && i != 27 && i != 28 && i != 30) { /* no unsupported regs */
                System.out.print(String.format("%s = %d\t\t", getRegName(i), this.rf[i]));
                indCount++;
                if (indCount % 4 == 0)
                    System.out.println();
            }
        }
        System.out.println("\n");
    }

    private String getRegName(int num) {
        /* gets name of input register number
         * basically inverse of lab2 getReg */
        switch(num) {
            case 0: return "$0";
            case 2: return "$v0";
            case 3: return "$v1";
            case 4: return "$a0";
            case 5: return "$a1";
            case 6: return "$a2";
            case 7: return "$a3";
            case 8: return "$t0";
            case 9: return "$t1";
            case 10: return "$t2";
            case 11: return "$t3";
            case 12: return "$t4";
            case 13: return "$t5";
            case 14: return "$t6";
            case 15: return "$t7";
            case 16: return "$s0";
            case 17: return "$s1";
            case 18: return "$s2";
            case 19: return "$s3";
            case 20: return "$s4";
            case 21: return "$s5";
            case 22: return "$s6";
            case 23: return "$s7";
            case 24: return "$t8";
            case 25: return "$t9";
            case 29: return "$sp";
            case 31: return "$ra";
            default: System.out.println(num + ": Invalid Register");
                return null;
        }
    }

    private int execInstr(instr it) {
        /* executes assembly instruction and stores data where appropriate
         * returns 1 if instr jumps, 0 otherwise*/
        /* troubleshooting
         *System.out.print(String.format("pc: %d, name: %s, line: %d, op: %d, bool: %b",
         *        pc, it.label, it.addr, it.op, it.isInstr));
         *for(int i = 0; i < it.args.length; i++) {
         *    System.out.print(String.format(" i=%d: arg=%d,", i, it.args[i]));
         *}
         *System.out.println();
         * end troubleshooting */
        if(it.label.equals("empty"))
            return 4;
        else if(it.label.equals("stall"))
            return 0;
        else if(it.label.equals("squash"))
            return 0;
        switch(it.op) {
            case 0: switch(it.args[4]) {
                case 0: rf[it.args[2]] = rf[it.args[1]] << it.args[3]; /* sll */
                    return 0;
                case 8: pc = rf[it.args[0]]; /* jr */
                    return 2;
                case 32: rf[it.args[2]] = rf[it.args[0]] + rf[it.args[1]]; /* add */
                    return 0;
                case 34: rf[it.args[2]] = rf[it.args[0]] - rf[it.args[1]]; /* sub */
                    return 0;
                case 36: rf[it.args[2]] = rf[it.args[0]] & rf[it.args[1]]; /* and */
                    return 0;
                case 37: rf[it.args[2]] = rf[it.args[0]] | rf[it.args[1]]; /* or */
                    return 0;
                case 42: if(rf[it.args[0]] < rf[it.args[1]]) /* slt*/
                        rf[it.args[2]] = 1;
                    else
                        rf[it.args[2]] = 0;
                    //if(it.args[0] == 4)
                        //System.out.println("a0: " + rf[it.args[0]]);
                    return 0;
                default: System.out.println(it.label + ": Invalid Instruction");
                    return 0;
            }
            case 2: pc = it.args[0]; /* j */
                //System.out.println("pc args: " + it.args[0]);
                return 2;
            case 3: //rf[31] = pc + 1; /* jal */
                rf[31] = pc;
                pc = it.args[0];
                return 2;
            case 4: if(rf[it.args[0]] == rf[it.args[1]]) { /* beq */
                pc++;
                return 1;
            } else
                return 0;
            case 5: if(rf[it.args[0]] != rf[it.args[1]]) { /* bne */
                pc++;
                return 1;
            } else
                return 0;
            case 8: rf[it.args[1]] = rf[it.args[0]] + it.args[2]; /* addi */
                return 0;
            case 35: rf[it.args[1]] = dm[rf[it.args[0]] + it.args[2]]; /* lw */
                return 0;
            case 43: dm[rf[it.args[0]] + it.args[2]] = rf[it.args[1]]; /* sw */
                return 0;
            default: System.out.println(it.label + ": Invalid Instruction");
                return 0;
        }
    }

    public static void main(String[] args) throws FileNotFoundException {
        /* main program */
        String filename = args[0];
        lab2 assembler = new lab2();
        lab3 emulator = new lab3();
        instrHt ht = assembler.passOne(filename);
        assembler.passTwo(filename, ht);
        instr[] instrArr = emulator.sortInstrs(ht);
        lab4 mainProg = new lab4(instrArr);
        if(args.length == 2) {
            mainProg.scriptMode(instrArr, args[1]);
        } else if (args.length == 1) {
            mainProg.shellMode(instrArr);
        } else {
            System.out.println("Invalid arguments");
            System.exit(1);
        }
    }
}