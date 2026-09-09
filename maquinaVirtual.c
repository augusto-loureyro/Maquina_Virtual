#include "maquinaVirtual.h"



void mvInic(ETMaquinaVirtual *maqVirt, bool modoDebug) {
    int i;

    /// Se inicializa la memoria con 0 para que no haya trash.
    memset(&(maqVirt->memoria), 0, sizeof(maqVirt->memoria));

    /// Se inicializan variables de control
    maqVirt->modoDebug = modoDebug;
    maqVirt->error = false;
    maqVirt->running = false;

    /// Inicializar los 32 registros
    for (i = 0; i < REGISTROS; i++)
        maqVirt->registros[i] = 0;

    /// Inicializar las tablas de los 8 segmentos
    for (i = 0; i < SEGMENTOS; i++) {
        maqVirt->segTabla[i].base = 0xFFFF;
        maqVirt->segTabla[i].dim = 0xFFFF;
    }
}

void mvError(ETMaquinaVirtual *maqVirt,char *mensaje) {
    printf("ERROR: %s\n", mensaje);
    maqVirt->error = true;
    maqVirt->running = false;
}

void setFlags(ETMaquinaVirtual *maqVirt, bool n, bool z, bool c, bool o) {
    int cc = 0;
    if(n)
        cc |= NMASK;
    if(z)
        cc |= ZMASK;
    if(c)
        cc |= CMASK;
    if(o)
        cc |= OMASK;
    maqVirt->registros[REGCC] = cc;
}

bool cargarArchivo(ETMaquinaVirtual *maqVirt, char *nombreArchivo) {
    FILE *arch = NULL;
    uint8_t header[8];
    int tamanioCode;
    bool flag = false;

    arch = fopen(nombreArchivo, "rb");
    if (arch != NULL) {
        /// Leer la cabecera
        if (fread(header, sizeof(header), 1, arch) != 1) {
            printf("Cabecera invalida\n");
        }else
            if (memcmp(header, "VMX26", 5) != 0) { ///memcmp. Devuelve 0 si ambos bloques de memoria son exactamente iguales en su contenido.
                printf("Identificador de programa invalido\n");
            }else
                if (header[5] != 1)
                    printf("Version invalida\n");
                else{
                    /// ShiftL 8 bits del  primero y concateno con los del segundo
                    tamanioCode = (header[6] << 8) | header[7];

                    /// Inicializar tabla de segmentos
                    /// Segmento de Codigo
                    maqVirt->segTabla[0].base = 0;
                    maqVirt->segTabla[0].dim = tamanioCode;

                    /// Segmento de Datos
                    maqVirt->segTabla[1].base = tamanioCode;
                    maqVirt->segTabla[1].dim = DIMMEMORIA - tamanioCode;

                    /// Inicializar CS y DS
                    maqVirt->registros[REGCS] = 0x00000000;
                    maqVirt->registros[REGDS] = 0x00010000;

                    /// IP apunta al inicio del segmento de código
                    maqVirt->registros[REGIP] = maqVirt->registros[REGCS];

                    /// Cargar codigo en memoria física
                    fread(maqVirt->memoria, 1, DIMMEMORIA, arch);
                    fclose(arch);
                    flag = true;
                }
        fclose(arch);
        return flag;
    }else{
        printf("No se pudo abrir el archivo: %s\n", nombreArchivo);
        return flag;
    }
}

void mvEjecutar(ETMaquinaVirtual *maqVirt) {
    maqVirt->running = true;
    int32_t dirLogica, dirFisica, dirFisicaTem, lengInstr;
    TRInstruction inst;

    while (maqVirt->running && !(maqVirt->error)) {
        if (maqVirt->registros[REGIP] == 0xFFFFFFFF) {
            maqVirt->running = false; /// Detiene ejecucion.
        }else{
            dirLogica = maqVirt->registros[REGIP];
            dirFisica = cambioLogicFisic(maqVirt, dirLogica);

            if (dirFisica >= 0) {
                dirFisicaTem = dirFisica;

                /// Leemos la siguiente instrucción desde la memoria física
                inst = leerInstruccion(maqVirt, &dirFisicaTem);

                /// Actualizamos IP lógico
                lengInstr = dirFisicaTem - dirFisica;
                maqVirt->registros[REGIP] += lengInstr;

                /// Guardamos los registros OPA y OPB
                maqVirt->registros[REGOPC] = inst.operacion;
                maqVirt->registros[REGOP1] = inst.tipoOpA << 24;
                maqVirt->registros[REGOP1] = inst.tipoOpA | inst.opAValor;
                maqVirt->registros[REGOP2] = inst.tipoOpB << 24;
                maqVirt->registros[REGOP2] = inst.tipoOpB | inst.opBValor;


                /// Ejecutamos la instrucción
                ejecutarInstruction(maqVirt, inst);
            }else
                maqVirt->running = false; /// Fuera del segmento de código
        }
    }
}



void guardarResult(ETMaquinaVirtual *maqVirt, uint8_t tipoOp, int32_t valorOp, int32_t result) {
    int memoriaP = 0;
    int32_t dest = operandoDest(maqVirt, tipoOp, valorOp, &memoriaP);

    if (memoriaP) {
        writeMem(maqVirt, dest, result);
    } else {
        maqVirt->registros[dest] = result;
    }
}




void ejecutarInstruction(ETMaquinaVirtual *maqVirt,TRInstruction inst){
    int32_t valA = 0, valB = 0, result=0;
    bool n = false, z = false, c = false, o = false;
    bool modControl = false;


    if (inst.cantOperand == 2) {
        valB = evaluate_operand(maqVirt, inst.tipoOpB, inst.opBValor);
    }else
        if (inst.cantOperand > 0 && inst.operacion != 0x1D && inst.operacion != 0x1E) {
            /// LDL y LDH sobreescriben parcialmente, pero requieren lectura
            valA = evaluate_operand(maqVirt, inst.tipoOpA, inst.opA_value);
        }


    switch (inst.operacion) {
        /// Sin Operandos
        case 0x00: /// SYS
            exec_sys(maqVirt, valA);
            break;


        /// 1 Operandos
        case 0x01: /// JMP
            maqVirt->registros[REGIP] = inst.opAValor;
            break;
            case 0x02: /// JP
            if (((maqVirt->registros[REGCC] & NMASK) == 0) && ((maqVirt->registros[REGCC] & ZMASK) == 0))
                maqVirt->registros[REGIP] = inst.opAValor;
            break;
        case 0x03: /// JN
            if ((maqVirt->registros[REGCC] & NMASK) != 0)
                maqVirt->registros[REGIP] = inst.opAValor;
            break;
        case 0x04: /// JZ
            if ((maqVirt->registros[REGCC] & ZMASK) == 0)
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
                mvError(maqVirt, "División por cero");
                return;
            }
            result = valA / valB;
            guardarResult(maqVirt, inst.tipoOpA, inst.opAValor, result);
            n = (result < 0);
            z = (result == 0);
            c = 0;
            o = 0;
            modControl = true;
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
            guardarResult(maqVirt, inst.opA_value, inst.opAValor, valB);
            guardarResult(maqVirt, inst.opB_value, inst.opBValor, valA);
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
            mvError(maqVirt, "Instrucción inválida");
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











