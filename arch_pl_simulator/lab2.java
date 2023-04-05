/* Name: Igal Tamarkin
*  Section: 07/08
*  Lab 2: Assembler Program
*/

import java.io.File;
import java.io.FileNotFoundException;
import java.util.Arrays;
import java.util.Scanner;
import java.util.regex.Pattern;
import java.util.List;
import java.util.ArrayList;

public class lab2 {

    public instrHt passOne(String filename) {
        /* first pass on asm file, finding and storing labels*/
        instrHt ht = new instrHt(20);
        try {
            File af = new File(filename);
            Scanner rdr = new Scanner(af);
            /* assume first address is located at 0, go line by line */
            int addr = 0;
            while(rdr.hasNextLine()) {
                if(1 != pass1DataFormat(rdr.nextLine(), addr, ht))
                    addr += 1;
            }
            rdr.close();
            return ht;
        } catch (FileNotFoundException e) {
            System.out.println("File not found.");
            return null;
        }
    }

    public void passTwo(String filename, instrHt ht) {
        /* second pass on asm file, converting all instructions to machine code*/
        try {
            File af = new File(filename);
            Scanner rdr = new Scanner(af);
            /* assume first address is located at 0, go line by line*/
            int addr = 0, labelAddr;
            while(rdr.hasNextLine()) {
                /* process each line of file*/
                labelAddr = pass2DataFormat(rdr.nextLine(), addr, ht);
                /* if an instruction is found, increment addr*/
                if(labelAddr == -1)
                    addr++;
                else if(labelAddr == -2)
                    addr += 0;
            }
            rdr.close();
        } catch (FileNotFoundException e) {
            System.out.println("File not found.");
        }
    }

    private int pass1DataFormat(String data, int addr, instrHt ht) {
        /* parse input line for labels, store any found in hashtable*/
        boolean pass = false;
        String label = "";
        int comChk = 0;
        String[] dataArr = data.trim().split("");
        /* iterate through string by char to isolate labels*/
        for(String s:dataArr) {
            if(s.length() > 0) {
                char c = s.charAt(0);
                if (Character.isLetterOrDigit(c)) {
                    label += c;
                } else if (c == ':') {
                    pass = true;
                    break;
                } else if(c == ' ' || c == 9) {
                    continue;
                } else if((c == '#') && s.equals(dataArr[0])) {
                    comChk = 1;
                    break;
                } else {
                    break;
                }
            }
        }
        if(pass) {
            /* if label is isolated, insert into hash table with line number*/
            instr lbl = new instr(label, addr);
            ht.insert(lbl);
        }
        if(dataArr.length == 1 && dataArr[0].length() == 0)
            comChk = 1;
        return comChk;
    }

    private int pass2DataFormat(String data, int addr, instrHt ht) {
        /* process input line into binary, output to stdout
         * if a label is encountered, return its address, otherwise return -1*/
        instr newInstr = null;
        int format = -1, i = 0;
        /* Split string by spaces*/
        String[] dataArr = data.trim().split("[\\s,]+");
        for (String s : dataArr) {
            if(s.length() == 0) {
                return -2;
            } if (s.equals("#") || s.contains("#")) {
                return -2;
            } else if(s.equals(" ") || s.contains(" ") || s.equals("    ") || s.contains("  ")) {
                return -2;
            } else if (s.length() > 1 && -1 != ht.getLabelAddrByName(s.substring(0, s.length() - 1))) {
                /* if label, cut it from the string to parse command, assuming the label is the first entry*/
                if(dataArr.length == 1 && -1 != ht.getLabelAddrByName(s.substring(0, s.length() - 1))) {
                    format = 3;
                    return -2;
                } else
                    dataArr = Arrays.copyOfRange(dataArr, 1, dataArr.length);
            } else {
                /* if instr and first register are concatenated, separate the two, then proceed*/
                if(s.contains("$")) {
                    List<String> dataTemp = new ArrayList<>(Arrays.asList(dataArr));
                    dataTemp.add(i+1, s.substring(s.indexOf('$')));
                    dataArr = dataTemp.toArray(dataArr);
                    s = s.substring(0, s.indexOf('$'));
                } else if(s.contains(":")) {
                    /* if a label and instr are concatenated, separate and proceed*/
                    dataArr[i] = s.substring(s.indexOf(':')+1);
                    s = dataArr[i];
                }
                /*parse line to see if it is a valid instruction, then
                 * create new instr object if it is */
                if(s.equals("and")) {
                    parseInstr(dataArr, 0, ht);
                    newInstr = new instr(dataArr[0], addr, 0, getReg(dataArr[2]),
                            getReg(dataArr[3]), getReg(dataArr[1]), 0, 36);
                    ht.insert(newInstr);
                    ht.finAddr++;
                    format = 0;
                } else if(s.equals("or")) {
                    parseInstr(dataArr, 0, ht);
                    newInstr = new instr(dataArr[0], addr, 0, getReg(dataArr[2]),
                            getReg(dataArr[3]), getReg(dataArr[1]), 0, 37);
                    ht.insert(newInstr);
                    ht.finAddr++;
                    format = 0;
                } else if(s.equals("add")) {
                    parseInstr(dataArr, 0, ht);
                    newInstr = new instr(dataArr[0], addr, 0, getReg(dataArr[2]),
                            getReg(dataArr[3]), getReg(dataArr[1]), 0, 32);
                    ht.insert(newInstr);
                    ht.finAddr++;
                    format = 0;
                } else if(s.equals("addi")) {
                    parseInstr(dataArr, 1, ht);
                    newInstr = new instr(dataArr[0], addr, 8, getReg(dataArr[2]),
                            getReg(dataArr[1]), Integer.parseInt(dataArr[3]));
                    ht.insert(newInstr);
                    ht.finAddr++;
                    format = 1;
                } else if(s.equals("sll")) {
                    parseInstr(dataArr, 0, ht);
                    newInstr = new instr(dataArr[0], addr, 0, 0,
                            getReg(dataArr[2]), getReg(dataArr[1]), Integer.parseInt(dataArr[3]), 0);
                    ht.insert(newInstr);
                    ht.finAddr++;
                    format = 0;
                } else if(s.equals("sub")) {
                    parseInstr(dataArr, 0, ht);
                    newInstr = new instr(dataArr[0], addr, 0, getReg(dataArr[2]),
                            getReg(dataArr[3]), getReg(dataArr[1]), 0, 34);
                    ht.insert(newInstr);
                    ht.finAddr++;
                    format = 0;
                } else if(s.equals("slt")) {
                    parseInstr(dataArr, 0, ht);
                    newInstr = new instr(dataArr[0], addr, 0, getReg(dataArr[2]),
                            getReg(dataArr[3]), getReg(dataArr[1]), 0, 42);
                    ht.insert(newInstr);
                    ht.finAddr++;
                    format = 0;
                } else if(s.equals("beq")) {
                    parseInstr(dataArr, 1, ht);
                    newInstr = new instr(dataArr[0], addr, 4, getReg(dataArr[1]),
                            getReg(dataArr[2]), Integer.parseInt(dataArr[3])-1-addr);
                    ht.insert(newInstr);
                    ht.finAddr++;
                    format = 1;
                } else if(s.equals("bne")) {
                    parseInstr(dataArr, 1, ht);
                    newInstr = new instr(dataArr[0], addr, 5, getReg(dataArr[1]),
                            getReg(dataArr[2]), Integer.parseInt(dataArr[3])-1-addr);
                    ht.insert(newInstr);
                    ht.finAddr++;
                    format = 1;
                } else if(s.equals("lw")) {
                    parseInstr(dataArr, 1, ht);
                    newInstr = new instr(dataArr[0], addr, 35,
                            getReg(dataArr[2].substring(dataArr[2].indexOf('$'), dataArr[2].indexOf(')'))),
                            getReg(dataArr[1]), Integer.parseInt(dataArr[2].substring(0, dataArr[2].indexOf('('))));
                    ht.insert(newInstr);
                    ht.finAddr++;
                    format = 1;
                } else if(s.equals("sw")) {
                    parseInstr(dataArr, 1, ht);
                    newInstr = new instr(dataArr[0], addr, 43,
                            getReg(dataArr[2].substring(dataArr[2].indexOf('$'), dataArr[2].indexOf(')'))),
                            getReg(dataArr[1]), Integer.parseInt(dataArr[2].substring(0, dataArr[2].indexOf('('))));
                    ht.insert(newInstr);
                    ht.finAddr++;
                    format = 1;
                } else if(s.equals("j")) {
                    parseInstr(dataArr, 2, ht);
                    newInstr = new instr("j", addr, 2, ht.getLabelAddrByName(dataArr[1]));
                    ht.insert(newInstr);
                    ht.finAddr++;
                    format = 2;
                } else if(s.equals("jr")) {
                    parseInstr(dataArr, 0, ht);
                    newInstr = new instr(dataArr[0], addr, 0, getReg(dataArr[1]),
                            0, 0, 0, 8);
                    ht.insert(newInstr);
                    ht.finAddr++;
                    format = 0;
                } else if(s.equals("jal")) {
                    parseInstr(dataArr, 2, ht);
                    newInstr = new instr("jal", addr, 3, ht.getLabelAddrByName(dataArr[1]));
                    ht.insert(newInstr);
                    ht.finAddr++;
                    format = 2;
                } else {
                    System.out.println(s + ": Invalid Instruction");
                    System.exit(1);
                }
                break;
            }
            i++;
        }
        /* print instr to stdout as binary sequence*/
        /*String op, rs, rt, rd, imm, shamt, funct;
        if (format == 0) {
            op = binPad(Integer.toBinaryString(newInstr.op), 6);
            rs = binPad(Integer.toBinaryString(newInstr.args[0]), 5);
            rt = binPad(Integer.toBinaryString(newInstr.args[1]), 5);
            rd = binPad(Integer.toBinaryString(newInstr.args[2]), 5);
            shamt = binPad(Integer.toBinaryString(newInstr.args[3]), 5);
            funct = binPad(Integer.toBinaryString(newInstr.args[4]), 6);
            System.out.println(op + " " + rs + " " + rt + " " + rd + " " + shamt + " " + funct);
        } else if (format == 1) {
            op = binPad(Integer.toBinaryString(newInstr.op), 6);
            rs = binPad(Integer.toBinaryString(newInstr.args[0]), 5);
            rt = binPad(Integer.toBinaryString(newInstr.args[1]), 5);
            imm = binPad(Integer.toBinaryString(newInstr.args[2]), 16);
            System.out.println(op + " " + rs + " " + rt + " " + imm);
        } else if (format == 2) {
            op = binPad(Integer.toBinaryString(newInstr.op), 6);
            imm = binPad(Integer.toBinaryString(newInstr.args[0]), 26);
            System.out.println(op + " " + imm);
        } else if(format != 3) {
            System.out.println("Invalid format");
            System.exit(1);
        }*/

        /* return jump address if applicable, return -1 otherwise*/
        return -1;
    }

    private int getReg(String reg) {
        /* returns the int associated with the integer*/
        switch(reg) {
            case "$0":
            case "$zero": return 0;
            case "$v0": return 2;
            case "$v1": return 3;
            case "$a0": return 4;
            case "$a1": return 5;
            case "$a2": return 6;
            case "$a3": return 7;
            case "$t0": return 8;
            case "$t1": return 9;
            case "$t2": return 10;
            case "$t3": return 11;
            case "$t4": return 12;
            case "$t5": return 13;
            case "$t6": return 14;
            case "$t7": return 15;
            case "$s0": return 16;
            case "$s1": return 17;
            case "$s2": return 18;
            case "$s3": return 19;
            case "$s4": return 20;
            case "$s5": return 21;
            case "$s6": return 22;
            case "$s7": return 23;
            case "$t8": return 24;
            case "$t9": return 25;
            case "$sp": return 29;
            case "$ra": return 31;
            default: System.out.println(reg + ": Invalid Register");
                System.exit(1);
        }
        return -1;
    }

    private void parseInstr(String[] instr, int format, instrHt ht) {
        /* checks to see if a line of instruction is valid based on the instruction format*/
        boolean pass = true;
        int i;
        Pattern pattern = Pattern.compile(".*[^0-9].*");
        if(format == 0) {
            for (i = 1; i < instr.length; i++) {/* r-type*/
                if (i > 3 && instr[i].charAt(0) != '#') {
                    pass = false;
                    break;
                } else if (instr[i].charAt(0) != '$' && instr[i].charAt(0) != '#') {
                    if (!instr[0].equals("sll")) {
                        pass = false;
                        break;
                    }
                } else if (instr[i].contains("#")) {
                    instr[i] = instr[i].substring(0, instr[i].indexOf('#'));
                    break;
                }
            }
        } else if(format == 1) {
            for (i = 1; i < instr.length; i++) {/* i-type*/
                if (i > 3 && instr[i].charAt(0) != '#') {
                    pass = false;
                    break;
                } else if (i < 3 && !instr[i].contains("$")) {
                    pass = false;
                    break;
                } else if(-1 != ht.getLabelAddrByName(instr[i])) {
                    /* if label is found, convert it to associated address*/
                    instr[i] = Integer.toString(ht.getLabelAddrByName(instr[i]));
                } else if (i == 3 && pattern.matcher(instr[i]).matches() && !instr[i].contains("#")
                        && instr[i].charAt(0) != '-' && !instr[0].equals("lw") && !instr[0].equals("sw")) {
                    pass = false;
                    break;
                } else if (instr[i].contains("#")) {
                    instr[i] = instr[i].substring(0, instr[i].indexOf('#'));
                    break;
                }
            }
        } else if(format == 2) {
            for (i = 1; i < instr.length; i++) {/* j-type*/
                if ((i > 1 && instr[i].charAt(0) != '#') || (i == 1 && instr[i].charAt(0) == '#')) {
                    pass = false;
                    break;
                } else if (i == 1 && -1 == ht.getLabelAddrByName(instr[i])) {
                    pass = false;
                    break;
                } else if (instr[i].contains("#")) {
                    instr[i] = instr[i].substring(0, instr[i].indexOf('#'));
                    break;
                }
            }
        } else
            pass = false;
        if(!pass) {
            System.out.println(instr[0] + ": Invalid Instruction");
            System.exit(1);
        }
    }

    private String binPad(String binString, int bitCount) {
        /* pads binary numbers to appropriate amount of bits
        * if 2's complement occurs, also trims bits starting from MSB*/
        String retString = "";
        if(binString.length() > bitCount) {
            while(binString.length() > bitCount) {
                binString = binString.substring(1);
            }
        } else {
            for (int i = 0; i < bitCount - binString.length(); i++)
                retString += "0";
        }
        return retString + binString;
    }


    public static void main(String[] args) {
        /* main program*/
        lab2 mainProg = new lab2();
        String filename = args[0];
        instrHt ht = mainProg.passOne(filename);
        mainProg.passTwo(filename, ht);
    }

}
