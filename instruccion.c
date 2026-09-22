#include "instruccion.h"

void ejecutarInstruccion(ETMaquinaVirtual *maqVirt) {
    int valA, valB, tipoOpA, tipoOpB, result;

    tipoOpA = (maqVirt->registros[REGOP1] >> 24) & 0xFF;
    tipoOpB = (maqVirt->registros[REGOP1] >> 24) & 0xFF;
    /// Obtener los valores almacenados en los operandos
    if(tipoOpA != OPNONE)
        valA = obtenerValorOperando(maqVirt, tipoOpA, maqVirt->registros[REGOP1]);
    if(tipoOpB != OPNONE)
        valB = obtenerValorOperando(maqVirt, tipoOpB, maqVirt->registros[REGOP2]);

    /// Si hubo fallo de segmento (en readMem)
    if (!maqVirt->running)
        return;

    switch (maqVirt->registros[REGOPC]) {
        /// 1 operando
        case 0x00: /// SYS
            llamadaSistema(maqVirt, valA);
            break;
        case 0x01: /// JMP
            maqVirt->registros[REGIP] = (maqVirt->registros[REGCS] >> 16) + valA;
            break;
        case 0x02: /// JP
            if (((maqVirt->registros[REGCC] & NMASK) == 0) && ((maqVirt->registros[REGCC] & ZMASK) == 0))
                maqVirt->registros[REGIP] = (maqVirt->registros[REGCS] >> 16) + valA;
            break;
        case 0x03: /// JN
            if ((maqVirt->registros[REGCC] & NMASK) != 0)
                maqVirt->registros[REGIP] = (maqVirt->registros[REGCS] >> 16) + valA;
            break;
        case 0x04: /// JZ
            if ((maqVirt->registros[REGCC] & ZMASK) != 0)
                maqVirt->registros[REGIP] = (maqVirt->registros[REGCS] >> 16) + valA;
            break;
        case 0x05: /// JC
            if ((maqVirt->registros[REGCC] & CMASK) != 0)
                maqVirt->registros[REGIP] = (maqVirt->registros[REGCS] >> 16) + valA;
            break;
        case 0x06: /// JV
            if ((maqVirt->registros[REGCC] & OMASK) != 0)
                maqVirt->registros[REGIP] = (maqVirt->registros[REGCS] >> 16) + valA;
            break;
        case 0x07: /// JNP
            if ((maqVirt->registros[REGCC] & NMASK) != 0 || (maqVirt->registros[REGCC] & ZMASK) != 0)
                maqVirt->registros[REGIP] = (maqVirt->registros[REGCS] >> 16) + valA;
            break;
        case 0x08: /// JNN
            if ((maqVirt->registros[REGCC] & NMASK) == 0)
                maqVirt->registros[REGIP] = (maqVirt->registros[REGCS] >> 16) + valA;
            break;
        case 0x09: /// JNZ
            if ((maqVirt->registros[REGCC] & ZMASK) == 0)
                maqVirt->registros[REGIP] = (maqVirt->registros[REGCS] >> 16) + valA;
            break;
        case 0x0A: /// NOT
            result = ~valA;
            guardarResult(maqVirt, tipoOpA, valA, result);
            setFlags(maqVirt, result < 0, result == 0, false, false);
            break;
        /// 0 operandos
        case 0x0F: /// STOP
            maqVirt->registros[REGIP] = 0xFFFFFFFF;
            break;
        /// 2 Operandos
        case 0x10: /// MOV
            result = valB;
            guardarResult(maqVirt, tipoOpA, valA, result);
            setFlags(maqVirt, result < 0, result == 0, false, false);
            break;
        case 0x11: /// ADD
            result = valA + valB;
            guardarResult(maqVirt, tipoOpA, valA, result);
            setFlags(maqVirt, result < 0, result == 0, result < valA, verificarOverflow(valA,valB,result,'+'));
            break;
        case 0x12: /// SUB
            result = valA - valB;
            guardarResult(maqVirt, tipoOpA, valA, result);
            setFlags(maqVirt, result < 0, result == 0, (valA + ~valB + 1) < valA || (~valB + 1) < ~valB,
                     verificarOverflow(valA,valB,result,'-'));
            break;
        case 0x13: { /// MUL
            long long int prod = (long long int)valA * (long long int)valB;
            result = (int)prod;
            guardarResult(maqVirt, tipoOpA, valA, result);
            setFlags(maqVirt, result < 0, result == 0, prod > 0xFFFFFFFF, verificarOverflow(valA,valB,result,'*'));
            break;
        }
        case 0x14: /// DIV
            if (valB == 0) {
                mvError(maqVirt, "Division por cero");
            }else{
                result = valA / valB;
                maqVirt->registros[REGAC] = valA % valB;
                guardarResult(maqVirt, tipoOpA, valA, result);
                setFlags(maqVirt, result < 0, result == 0, false, false);
            }
            break;
        case 0x15: /// CMP
            result = valA - valB;
            setFlags(maqVirt, result < 0, result == 0, valA >= valB, verificarOverflow(valA,valB,result,'-'));
            break;
        case 0x16: /// AND
            result = valA & valB;
            guardarResult(maqVirt, tipoOpA, valA, result);
            setFlags(maqVirt, result < 0, result == 0, false, false);
            break;
        case 0x17: /// OR
            result = valA | valB;
            guardarResult(maqVirt, tipoOpA, valA, result);
            setFlags(maqVirt, result < 0, result == 0, false, false);
            break;
        case 0x18: /// XOR
            result = valA ^ valB;
            guardarResult(maqVirt, tipoOpA, valA, result);
            setFlags(maqVirt, result < 0, result == 0, false, false);
            break;
        case 0x19: /// SWAP
            guardarResult(maqVirt, tipoOpA, valA, valB);
            guardarResult(maqVirt, tipoOpB, valB, valA);
            result = valA ^ valB;
            setFlags(maqVirt, result < 0, result == 0, false, false);
            break;
        case 0x1A: /// SHL
            if (valB > 0 && valB < 32) {
                result = valA << valB;
                guardarResult(maqVirt, tipoOpA, valA, result);
                setFlags(maqVirt, result < 0, result == 0, false, valA < 0);
            }
            break;
        case 0x1B: /// SHR
            if (valB > 0 && valB < 32) {
                result = (unsigned int)valA >> valB;
                guardarResult(maqVirt, tipoOpA, valA, result);
                setFlags(maqVirt, result < 0, result == 0, false, valA < 0);
            }
            break;
        case 0x1C: /// SAR (Shift Aritmetico)
            if (valB > 0 && valB < 32) {
                result = valA >> valB;
                guardarResult(maqVirt, tipoOpA, valA, result);
                setFlags(maqVirt, result < 0, result == 0, valA >> (valB - 1) & 1, false);
            }
            break;
        case 0x1D: /// LDL
            result = (valA & 0xFFFF0000) | (valB & 0xFFFF);
            guardarResult(maqVirt, tipoOpA, valA, result);
            break;
        case 0x1E: /// LDH
            result = (valA & 0x0000FFFF) | (valB << 16);
            guardarResult(maqVirt, tipoOpA, valA, result);
            break;
        case 0x1F: /// RND
            if (valB >= 0)
                result = rand() % (valB + 1);
            else
                result = -(rand() % (-valB + 1));
            guardarResult(maqVirt, tipoOpA, valA, result);
            break;
        default:
            mvError(maqVirt, "Instruccion invalida");
            break;
    }
}


bool verificarOverflow(int valA, int valB, int resultado, char op) {
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
            if (valA == 0 || valB == 0) return false;
            if ((valA == -1 && valB == -2147483648) || (valB == -1 && valA == -2147483648))
                return true;
            else/// Si al despejar valB el resultado no coincide, los bits se truncaron
                return (resultado / valA) != valB;
        default:
            return false;
    }
}

