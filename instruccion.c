#include "instruccion.h"

void ejecutarInstruccion(ETMaquinaVirtual *maqVirt) {
    int valA, valB, tipoOpA, tipoOpB, result;

    tipoOpA = (maqVirt->registros[REGOP1] >> 24) & 0xFF;
    tipoOpB = (maqVirt->registros[REGOP2] >> 24) & 0xFF;
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
        case SYS:
            llamadaSistema(maqVirt, valA);
            break;
        case JMP:
            maqVirt->registros[REGIP] = (maqVirt->registros[REGCS] & 0xFFFF0000) | (valA & 0xFFFF);
            break;
        case JP:
            if (((maqVirt->registros[REGCC] & NMASK) == 0) && ((maqVirt->registros[REGCC] & ZMASK) == 0))
                maqVirt->registros[REGIP] = (maqVirt->registros[REGCS] & 0xFFFF0000) | (valA & 0xFFFF);
            break;
        case JN:
            if ((maqVirt->registros[REGCC] & NMASK) != 0)
                maqVirt->registros[REGIP] = (maqVirt->registros[REGCS] & 0xFFFF0000) | (valA & 0xFFFF);
            break;
        case JZ:
            if ((maqVirt->registros[REGCC] & ZMASK) != 0)
                maqVirt->registros[REGIP] = (maqVirt->registros[REGCS] & 0xFFFF0000) | (valA & 0xFFFF);
            break;
        case JC:
            if ((maqVirt->registros[REGCC] & CMASK) != 0)
                maqVirt->registros[REGIP] = (maqVirt->registros[REGCS] & 0xFFFF0000) | (valA & 0xFFFF);
            break;
        case JV:
            if ((maqVirt->registros[REGCC] & OMASK) != 0)
                maqVirt->registros[REGIP] = (maqVirt->registros[REGCS] & 0xFFFF0000) | (valA & 0xFFFF);
            break;
        case JNP:
            if ((maqVirt->registros[REGCC] & NMASK) != 0 || (maqVirt->registros[REGCC] & ZMASK) != 0)
                maqVirt->registros[REGIP] = (maqVirt->registros[REGCS] & 0xFFFF0000) | (valA & 0xFFFF);
            break;
        case JNN:
            if ((maqVirt->registros[REGCC] & NMASK) == 0)
                maqVirt->registros[REGIP] = (maqVirt->registros[REGCS] & 0xFFFF0000) | (valA & 0xFFFF);
            break;
        case JNZ:
            if ((maqVirt->registros[REGCC] & ZMASK) == 0)
                maqVirt->registros[REGIP] = (maqVirt->registros[REGCS] & 0xFFFF0000) | (valA & 0xFFFF);
            break;
        case NOT:
            result = ~valA;
            guardarResult(maqVirt, tipoOpA, maqVirt->registros[REGOP1], result);
            setFlags(maqVirt, result < 0, result == 0, false, false);
            break;
        /// 0 operandos
        case STOP:
            maqVirt->registros[REGIP] = 0xFFFFFFFF;
            break;
        /// 2 Operandos
        case MOV:
            result = valB;
            guardarResult(maqVirt, tipoOpA, maqVirt->registros[REGOP1], result); //envio operando codificado (NO su valor)
            setFlags(maqVirt, result < 0, result == 0, false, false);
            break;
        case ADD:
            result = valA + valB;
            guardarResult(maqVirt, tipoOpA, maqVirt->registros[REGOP1], result);
            setFlags(maqVirt, result < 0, result == 0, result < valA, verificarOverflow(valA,valB,result,'+'));
            break;
        case SUB:
            result = valA - valB;
            guardarResult(maqVirt, tipoOpA, maqVirt->registros[REGOP1], result);
            setFlags(maqVirt, result < 0, result == 0, (valA + ~valB + 1) < valA || (~valB + 1) < ~valB,
                     verificarOverflow(valA,valB,result,'-'));
            break;
        case MUL: {
            long long int prod = (long long int)valA * (long long int)valB;
            result = (int)prod;
            guardarResult(maqVirt, tipoOpA, maqVirt->registros[REGOP1], result);
            setFlags(maqVirt, result < 0, result == 0, prod > 0xFFFFFFFF, verificarOverflow(valA,valB,result,'*'));
            break;
        }
        case DIV:
            if (valB == 0) {
                mvError(maqVirt, "Division por cero");
            }else{
                result = valA / valB;
                maqVirt->registros[REGAC] = valA % valB;
                guardarResult(maqVirt, tipoOpA, maqVirt->registros[REGOP1], result);
                setFlags(maqVirt, result < 0, result == 0, false, false);
            }
            break;
        case CMP:
            result = valA - valB;
            setFlags(maqVirt, result < 0, result == 0, valA >= valB, verificarOverflow(valA,valB,result,'-'));
            break;
        case AND:
            result = valA & valB;
            guardarResult(maqVirt, tipoOpA, maqVirt->registros[REGOP1], result);
            setFlags(maqVirt, result < 0, result == 0, false, false);
            break;
        case OR:
            result = valA | valB;
            guardarResult(maqVirt, tipoOpA, maqVirt->registros[REGOP1], result);
            setFlags(maqVirt, result < 0, result == 0, false, false);
            break;
        case XOR:
            result = valA ^ valB;
            guardarResult(maqVirt, tipoOpA, maqVirt->registros[REGOP1], result);
            setFlags(maqVirt, result < 0, result == 0, false, false);
            break;
        case SWAP:
            guardarResult(maqVirt, tipoOpA, maqVirt->registros[REGOP1], valB);
            guardarResult(maqVirt, tipoOpB, maqVirt->registros[REGOP2], valA);
            result = valA ^ valB;
            setFlags(maqVirt, result < 0, result == 0, false, false);
            break;
        case SHL:
            if (valB > 0 && valB < 32) {
                result = valA << valB;
                guardarResult(maqVirt, tipoOpA, maqVirt->registros[REGOP1], result);
                setFlags(maqVirt, result < 0, result == 0, false, valA < 0);
            }
            break;
        case SHR:
            if (valB > 0 && valB < 32) {
                result = (unsigned int)valA >> valB;
                guardarResult(maqVirt, tipoOpA, maqVirt->registros[REGOP1], result);
                setFlags(maqVirt, result < 0, result == 0, false, valA < 0);
            }
            break;
        case SAR: /// Shift Aritmetico
            if (valB > 0 && valB < 32) {
                result = valA >> valB;
                guardarResult(maqVirt, tipoOpA, maqVirt->registros[REGOP1], result);
                setFlags(maqVirt, result < 0, result == 0, valA >> (valB - 1) & 1, false);
            }
            break;
        case LDL:
            result = (valA & 0xFFFF0000) | (valB & 0xFFFF);
            guardarResult(maqVirt, tipoOpA, maqVirt->registros[REGOP1], result);
            break;
        case LDH:
            result = (valA & 0x0000FFFF) | (valB << 16);
            guardarResult(maqVirt, tipoOpA, maqVirt->registros[REGOP1], result);
            break;
        case RND:
            if (valB >= 0)
                result = rand() % (valB + 1);
            else
                result = -(rand() % (-valB + 1));
            guardarResult(maqVirt, tipoOpA, maqVirt->registros[REGOP1], result);
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

