/* Name: Igal Tamarkin
 *  Section: 07/08
 *  Lab 3: MIPS Emulator
 */

import java.io.File;
import java.io.FileNotFoundException;
import java.util.Arrays;
import java.util.Scanner;
import java.util.regex.Pattern;
import java.util.List;
import java.util.ArrayList;

public class lab3 {

    private int pc;
    private int[] dm;
    private int[] rf;

    public lab3() {
        this.pc = 0;
        this.dm = new int[8192];
        this.rf = new int[32];
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
        int i, stepAmt, stepAmt2;
        switch (stdin[0]) {
            case "h": System.out.println("h = show help");
                System.out.println("d = dump register state");
                System.out.println("s = single step through the program (i.e. execute 1 instruction and stop)");
                System.out.println("s num = step through num instructions of the program");
                System.out.println("r = run until the program ends");
                System.out.println("m num1 num2 = display data memory from location num1 to num2");
                System.out.println("c = clear all registers, memory, and the program counter to 0");
                System.out.println("q = exit the program\n");
                break;
            case "d": dumpRegs();
                break;
            case "r": while(pc < instrArr.length) {
                    if(0 == execInstr(instrArr[pc]))
                        pc++;
                }
                System.out.println("\n");
                break;
            case "c": Arrays.fill(this.dm, 0);
                Arrays.fill(this.rf, 0);
                this.pc = 0;
                System.out.println("\tSimulator reset\n");
                break;
            case "s": if(stdin.length == 1)
                    stepAmt = 1;
                else
                    stepAmt = Integer.parseInt(stdin[1]);
                stepAmt2 = stepAmt;
                while(stepAmt > 0) {
                    if(0 == execInstr(instrArr[pc]))
                        pc++;
                    stepAmt--;
                    if(pc > instrArr.length)
                        break;
                }
                System.out.println(String.format("\t%d instruction(s) executed\n", stepAmt2));
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
        switch(it.op) {
            case 0: switch(it.args[4]) {
                case 0: rf[it.args[2]] = rf[it.args[1]] << it.args[3]; /* sll */
                    return 0;
                case 8: pc = rf[it.args[0]]; /* jr */
                    return 1;
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
                    return 0;
                default: System.out.println(it.label + ": Invalid Instruction");
                    return 0;
                }
            case 2: pc = it.args[0]; /* j */
                return 1;
            case 3: rf[31] = pc + 1; /* jal */
                    pc = it.args[0];
                return 1;
            case 4: if(rf[it.args[0]] == rf[it.args[1]]) { /* beq */
                    pc += 1 + it.args[2];
                    return 1;
                } else
                    return 0;
            case 5: if(rf[it.args[0]] != rf[it.args[1]]) { /* bne */
                    pc += 1 + it.args[2];
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

    public instr[] sortInstrs(instrHt ht) {
        /* extract all instructions from the hashtable into a sorted array*/
        instr[] retArr = new instr[ht.finAddr];
        for(int i = 0; i < ht.finAddr; i++) {
            retArr[i] = ht.getByAddr(i);
        }
        return retArr;
    }

    public static void main(String[] args) throws FileNotFoundException {
        /* main program*/
        lab3 mainProg = new lab3();
        String filename = args[0];
        lab2 assembler = new lab2();
        instrHt ht = assembler.passOne(filename);
        assembler.passTwo(filename, ht);
        instr[] instrArr = mainProg.sortInstrs(ht);
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
