#include "instruccion.h"

void ejecutarInstruction(ETMaquinaVirtual *maqVirt,TRInstruction inst){
    int32_t valA = inst.opAValor, valB = inst.opBValor, result=0;
    bool n = false, z = false, c = false, o = false;
    bool modControl = false;

    /// Obtener los VALORES REALES desreferenciados de los operandos
    if (inst.cantOperand == 1) {
        valA = obtenerValorOperando(maqVirt, inst.tipoOpA, inst.opAValor);
    }else
        if (inst.cantOperand == 2) {
            valB = obtenerValorOperando(maqVirt, inst.tipoOpB, inst.opBValor);
            valA = obtenerValorOperando(maqVirt, inst.tipoOpA, inst.opAValor);
        }

    /// SI OBTENER EL VALOR FALLO (Fallo de segmento en readMem)
    if (!maqVirt->running || maqVirt->error) {
        return;
    }

    switch (inst.operacion) {
        /// Sin Operandos
        case 0x00: /// SYS
            llamadaSistema(maqVirt, valA);
            break;


        /// 1 Operandos
        case 0x01: /// JMP
            maqVirt->registros[REGIP] = inst.opAValor;
            break;
            case 0x02: /// JP
            if (((maqVirt->registros[REGCC] & NMASK) == 0) && ((maqVirt->registros[REGCC] & ZMASK) == 0))
                maqVirt->registros[REGIP] = inst.opAValor & 0xFFFF;
            break;
        case 0x03: /// JN
            if ((maqVirt->registros[REGCC] & NMASK) != 0)
                maqVirt->registros[REGIP] = inst.opAValor & 0xFFFF;
            break;
        case 0x04: /// JZ
            if ((maqVirt->registros[REGCC] & ZMASK) != 0)
                maqVirt->registros[REGIP] = inst.opAValor;
            break;
        case 0x05: /// JC
            if ((maqVirt->registros[REGCC] & CMASK) != 0)
                maqVirt->registros[REGIP] = inst.opAValor;
            break;
        case 0x06: /// JV
            if ((maqVirt->registros[REGCC] & OMASK) != 0)
                maqVirt->registros[REGIP] = inst.opAValor;
            break;
        case 0x07: /// JNP
            if ((maqVirt->registros[REGCC] & NMASK) != 0 || (maqVirt->registros[REGCC] & ZMASK) != 0)
                maqVirt->registros[REGIP] = inst.opAValor;
            break;
        case 0x08: /// JNN
            if ((maqVirt->registros[REGCC] & NMASK) == 0)
                maqVirt->registros[REGIP] = inst.opAValor;
            break;
        case 0x09: /// JNZ
            if ((maqVirt->registros[REGCC] & ZMASK) == 0)
                maqVirt->registros[REGIP] = inst.opAValor;
            break;
        case 0x0A: /// NOT
            result = ~inst.opAValor;
            guardarResult(maqVirt, inst.tipoOpA, inst.opAValor, result);
            n = (result < 0);
            z = (result == 0);
            c = 0;
            o = 0;
            modControl = true;
            break;
        case 0x0F: /// STOP
            maqVirt->registros[REGIP] = 0xFFFFFFFF;
            maqVirt->running = false;
            break;





        /// 2 Operandos
        case 0x10: /// MOV
            result = valB;
            guardarResult(maqVirt, inst.tipoOpA, inst.opAValor, result);
            n = (result < 0);
            z = (result == 0);
            c = 0;
            o = 0;
            modControl = true;
            break;
        case 0x11: /// ADD
            result = valA + valB;
            guardarResult(maqVirt, inst.tipoOpA, inst.opAValor, result);
            n = (result < 0);
            z = (result == 0);
            c = (result < valA);
            o = verificarOverflow(valA,valB,result,'+');
            modControl = true;
            break;
        case 0x12: /// SUB
            result = valA - valB;
            guardarResult(maqVirt, inst.tipoOpA, inst.opAValor, result);
            n = (result < 0);
            z = (result == 0);
            c = (valA + ~valB + 1) < valA || (~valB + 1) < ~valB;
            c = (valA >= valB);
            o = verificarOverflow(valA,valB,result,'-');
            modControl = true;
            break;
        case 0x13: { /// MUL
            long long prod = (long long)valA * (long long)valB;
            result = (int32_t)prod;
            guardarResult(maqVirt, inst.tipoOpA, inst.opAValor, result);
            n = (result < 0);
            z = (result == 0);
            c = (prod > 0xFFFFFFFF);
            o =  verificarOverflow(valA,valB,result,'*');
            modControl = true;
            break;
        }
        case 0x14: /// DIV
            if (valB == 0) {
                mvError(maqVirt, "Division por cero");
            }else{
                result = valA / valB;
                maqVirt->registros[REGAC] = valA % valB;
                guardarResult(maqVirt, inst.tipoOpA, inst.opAValor, result);
                n = (result < 0);
                z = (result == 0);
                c = 0;
                o = 0;
                modControl = true;
            }
            break;
        case 0x15: /// CMP
            result = valA - valB;
            n = (result < 0);
            z = (result == 0);
            c = (valA >= valB);
            o =  verificarOverflow(valA,valB,result,'-');
            modControl = true;
            break;
        case 0x16: /// AND
            result = valA & valB;
            guardarResult(maqVirt, inst.tipoOpA, inst.opAValor, result);
            n = (result < 0);
            z = (result == 0);
            c = 0;
            o = 0;
            modControl = true;
            break;
        case 0x17: /// OR
            result = valA | valB;
            guardarResult(maqVirt, inst.tipoOpA, inst.opAValor, result);
            n = (result < 0);
            z = (result == 0);
            c = 0;
            o = 0;
            modControl = true;
            break;
        case 0x18: /// XOR
            result = valA ^ valB;
            guardarResult(maqVirt, inst.tipoOpA, inst.opAValor, result);
            n = (result < 0);
            z = (result == 0);
            c = 0;
            o = 0;
            modControl = true;
            break;
        case 0x19: /// SWAP
            result = valB;
            guardarResult(maqVirt, inst.tipoOpA, inst.opAValor, valB);
            guardarResult(maqVirt, inst.tipoOpB, inst.opBValor, valA);
            n = (result < 0);
            z = (result == 0);
            c = 0;
            o = 0;
            modControl = true;
            break;
        case 0x1A: /// SHL
            if (valB < 32 && valB > 0) {
                result = valA << valB;
                c = (valA >> (32 - valB)) & 1;
            } else {
                /// Desplazamientos >= 32 limpian por completo el registro de 32 bits
                result = 0;
                c = 0;
            }
            guardarResult(maqVirt, inst.tipoOpA, inst.opAValor, result);
            n = (result < 0);
            z = (result == 0);
            o = n != (valA < 0);
            modControl = true;
            break;
        case 0x1B: /// SHR
            if (valB < 32 && valB > 0) {
                result = valA >> valB;
                c = (valA >> (valB - 1)) & 1;
            } else {
                result = 0;
                c = 0;
            }
            guardarResult(maqVirt, inst.tipoOpA, inst.opAValor, result);
            n = (result < 0);
            z = (result == 0);
            o = n != (valA < 0);
            modControl = true;
            break;
        case 0x1C: /// SAR (Shift Aritmetico)
            if(valB < 32 && valB > 0) {
                /// En C, si valA es int32_t (con signo), ">>" realiza un Shift Aritmetico
                result = valA >> valB;
                c = (valA >> (valB - 1)) & 1;
            }else{
                /// Para desplazamientos >= 32, el registro se llena completamente con el bit de signo
                result = (valA < 0) ? -1 : 0;
                c = (valA < 0) ? 1 : 0;
            }
            guardarResult(maqVirt, inst.tipoOpA, inst.opAValor, result);
            n = (result < 0);
            z = (result == 0);
            o = 0;
            modControl = true;
            break;
        case 0x1D: /// LDL
            result = (valA & 0xFFFF0000) | (valB & 0xFFFF);
            guardarResult(maqVirt, inst.tipoOpA, inst.opAValor, result);
            break;
        case 0x1E: /// LDH
            result = (valA & 0x0000FFFF) | (valB << 16);
            guardarResult(maqVirt, inst.tipoOpA, inst.opAValor, result);
            break;
        case 0x1F: /// RND
            result = (rand() % (valB + 1));
            guardarResult(maqVirt, inst.tipoOpA, inst.opAValor, result);
            break;
        default:
            mvError(maqVirt, "Instruccion invalida");
            break;
    }

    if(modControl)
        setFlags(maqVirt, n, z, c, o);
}






bool verificarOverflow(int32_t valA, int32_t valB, int32_t resultado, char op) {
    switch (op) {
        case '+':
            /// Positivo + Positivo = Negativo  OR  Negativo + Negativo = Positivo
            return (valA >= 0 && valB >= 0 && resultado < 0) ||
                   (valA < 0  && valB < 0  && resultado >= 0);

        case '-':
            /// Positivo - Negativo = Negativo  OR  Negativo - Positivo = Positivo
            return (valA >= 0 && valB < 0  && resultado < 0) ||
                   (valA < 0  && valB >= 0 && resultado >= 0);

        case '*':
            if (valA == 0 || valB == 0)
                return false;
            else
                if ((valA == -1 && valB == -2147483648) || (valB == -1 && valA == -2147483648))
                    return true;
                else/// Si al despejar valB el resultado no coincide, los bits se truncaron
                    return (resultado / valA) != valB;
        default:
            return false;
    }
}

