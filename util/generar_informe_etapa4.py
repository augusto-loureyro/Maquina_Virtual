import os
import sys
from reportlab.lib.pagesizes import A4
from reportlab.lib import colors
from reportlab.lib.styles import getSampleStyleSheet, ParagraphStyle
from reportlab.platypus import (
    SimpleDocTemplate, Paragraph, Spacer, Table, TableStyle, PageBreak, KeepTogether, HRFlowable
)
from reportlab.pdfgen import canvas

class NumberedCanvas(canvas.Canvas):
    def __init__(self, *args, **kwargs):
        super(NumberedCanvas, self).__init__(*args, **kwargs)
        self._saved_page_states = []

    def showPage(self):
        self._saved_page_states.append(dict(self.__dict__))
        self._startPage()

    def save(self):
        num_pages = len(self._saved_page_states)
        for state in self._saved_page_states:
            self.__dict__.update(state)
            self.draw_footer(num_pages)
            canvas.Canvas.showPage(self)
        canvas.Canvas.save(self)

    def draw_footer(self, page_count):
        self.saveState()
        self.setFont("Helvetica", 8)
        self.setFillColor(colors.HexColor("#555555"))
        self.setStrokeColor(colors.HexColor("#D0D0D0"))
        self.setLineWidth(0.5)
        self.line(36, 32, 559, 32)
        
        texto_izq = "Documento generado para el seguimiento del trabajo práctico de Máquina Virtual MV2 — UNMDP 2026."
        self.drawString(36, 22, texto_izq)
        
        texto_der = f"Página {self._pageNumber} de {page_count}"
        self.drawRightString(559, 22, texto_der)
        self.restoreState()

def generar_pdf(ruta_salida):
    doc = SimpleDocTemplate(
        ruta_salida,
        pagesize=A4,
        leftMargin=36,
        rightMargin=36,
        topMargin=36,
        bottomMargin=45
    )
    
    styles = getSampleStyleSheet()
    
    # Colores institucionales
    c_primary = colors.HexColor("#1A56DB")
    c_dark = colors.HexColor("#1E293B")
    c_text = colors.HexColor("#334155")
    c_light_bg = colors.HexColor("#F8FAFC")
    c_border = colors.HexColor("#CBD5E1")
    c_green = colors.HexColor("#16A34A")
    c_code_bg = colors.HexColor("#F1F5F9")
    
    # Estilos de parrafo
    s_title = ParagraphStyle(
        'DocTitle',
        parent=styles['Normal'],
        fontName='Helvetica-Bold',
        fontSize=18,
        leading=22,
        textColor=c_dark
    )
    
    s_subtitle = ParagraphStyle(
        'DocSubTitle',
        parent=styles['Normal'],
        fontName='Helvetica-Bold',
        fontSize=13,
        leading=16,
        textColor=c_primary
    )
    
    s_inst_left = ParagraphStyle(
        'InstLeft',
        parent=styles['Normal'],
        fontName='Helvetica',
        fontSize=8,
        leading=11,
        textColor=colors.HexColor("#64748B"),
        alignment=0
    )
    
    s_inst_right = ParagraphStyle(
        'InstRight',
        parent=styles['Normal'],
        fontName='Helvetica',
        fontSize=8,
        leading=11,
        textColor=colors.HexColor("#64748B"),
        alignment=2
    )
    
    s_h1 = ParagraphStyle(
        'Heading1_Custom',
        parent=styles['Normal'],
        fontName='Helvetica-Bold',
        fontSize=11,
        leading=14,
        textColor=c_primary,
        spaceBefore=8,
        spaceAfter=4
    )
    
    s_h2 = ParagraphStyle(
        'Heading2_Custom',
        parent=styles['Normal'],
        fontName='Helvetica-Bold',
        fontSize=9.5,
        leading=12,
        textColor=c_dark,
        spaceBefore=6,
        spaceAfter=3
    )
    
    s_body = ParagraphStyle(
        'Body_Custom',
        parent=styles['Normal'],
        fontName='Helvetica',
        fontSize=8.5,
        leading=11.5,
        textColor=c_text
    )
    
    s_bullet = ParagraphStyle(
        'Bullet_Custom',
        parent=s_body,
        leftIndent=12,
        bulletIndent=4,
        spaceBefore=1,
        spaceAfter=1
    )
    
    s_code = ParagraphStyle(
        'Code_Custom',
        parent=styles['Normal'],
        fontName='Courier',
        fontSize=7.5,
        leading=9.5,
        textColor=colors.HexColor("#0F172A")
    )
    
    s_table_hdr = ParagraphStyle(
        'TableHdr',
        parent=styles['Normal'],
        fontName='Helvetica-Bold',
        fontSize=8,
        leading=10,
        textColor=colors.white,
        alignment=1
    )
    
    s_table_cell = ParagraphStyle(
        'TableCell',
        parent=styles['Normal'],
        fontName='Helvetica',
        fontSize=7.5,
        leading=9.5,
        textColor=c_text
    )

    s_table_cell_center = ParagraphStyle(
        'TableCellCenter',
        parent=s_table_cell,
        alignment=1
    )

    s_table_cell_bold = ParagraphStyle(
        'TableCellBold',
        parent=styles['Normal'],
        fontName='Helvetica-Bold',
        fontSize=7.5,
        leading=9.5,
        textColor=c_dark
    )

    s_table_cell_ok = ParagraphStyle(
        'TableCellOk',
        parent=styles['Normal'],
        fontName='Helvetica-Bold',
        fontSize=7.5,
        leading=9.5,
        textColor=c_green,
        alignment=1
    )

    story = []

    # ------------------ ENCABEZADO ------------------
    header_data = [
        [
            Paragraph("<b>Máquina Virtual - Parte II (MV2)</b><br/><font color='#1A56DB'><b>Informe Técnico: Etapa 4 - Registros y Operandos MV2</b></font>", s_title),
            Paragraph("UNMDP - Facultad de Ingeniería<br/>Fundamentos de la Arq. de Computadoras", s_inst_right)
        ]
    ]
    t_header = Table(header_data, colWidths=[350, 173])
    t_header.setStyle(TableStyle([
        ('VALIGN', (0,0), (-1,-1), 'MIDDLE'),
        ('LEFTPADDING', (0,0), (-1,-1), 0),
        ('RIGHTPADDING', (0,0), (-1,-1), 0),
        ('BOTTOMPADDING', (0,0), (-1,-1), 4),
    ]))
    story.append(t_header)
    story.append(HRFlowable(width="100%", thickness=1, color=c_primary, spaceBefore=4, spaceAfter=6))

    # ------------------ BADGE Y METADATOS ------------------
    badge_data = [
        [
            Paragraph("<b>BASE DEL PROYECTO</b><br/>Etapa 3: Memoria variable y segmentos", s_table_cell),
            Paragraph("<b>ETAPA ACTUAL</b><br/><b>Etapa 4: Registros y operandos MV2</b>", s_table_cell_bold),
            Paragraph("<b>PRÓXIMA DEPENDENCIA</b><br/>Etapa 5: PUSH / POP / CALL / RET", s_table_cell),
            Paragraph("<b>ESTADO: COMPLETADA</b>", ParagraphStyle('Bdg', parent=s_table_hdr, fontName='Helvetica-Bold', fontSize=8, textColor=colors.white))
        ],
        [
            Paragraph("<b>COMPILADOR</b><br/>GCC (MinGW) / C99", s_table_cell),
            Paragraph("<b>FLAGS DE COMPILACIÓN</b><br/>-Wall -Wextra (0 warnings, 0 errors)", s_table_cell),
            Paragraph("<b>COMPATIBILIDAD</b><br/>100% retrocompatible con VMX1 y MV2", s_table_cell),
            Paragraph("<font color='#16A34A'><b>100% VALIDADA</b></font>", s_table_cell_center)
        ]
    ]
    t_badge = Table(badge_data, colWidths=[130, 140, 140, 113])
    t_badge.setStyle(TableStyle([
        ('BACKGROUND', (0,0), (-2,-1), c_light_bg),
        ('BACKGROUND', (3,0), (3,0), c_green),
        ('BACKGROUND', (3,1), (3,1), colors.HexColor("#DCFCE7")),
        ('BOX', (0,0), (-1,-1), 0.5, c_border),
        ('INNERGRID', (0,0), (-1,-1), 0.5, c_border),
        ('VALIGN', (0,0), (-1,-1), 'MIDDLE'),
        ('TOPPADDING', (0,0), (-1,-1), 3),
        ('BOTTOMPADDING', (0,0), (-1,-1), 3),
        ('LEFTPADDING', (0,0), (-1,-1), 5),
        ('RIGHTPADDING', (0,0), (-1,-1), 5),
    ]))
    story.append(t_badge)
    story.append(Spacer(1, 6))

    # ------------------ 1. OBJETIVO Y CRITERIOS ------------------
    story.append(Paragraph("1. Objetivo y Criterios de Finalización", s_h1))
    story.append(Paragraph(
        "El objetivo principal de la <b>Etapa 4</b> es actualizar de forma rigurosa y modular el <b>banco de registros</b> "
        "y el <b>decodificador de instrucciones</b> previo a la implementación del subsistema de pila (Etapa 5). "
        "Se introducen formalmente los registros de control y segmentos (SP, BP, ES, SS, KS, PS), la discriminación "
        "por sectores de los registros generales (EAX a EFX: registro completo, byte bajo, byte alto y word bajo), y la "
        "decodificación de operandos de memoria con modificadores de tamaño (b[], w[], l[] y [] por defecto).",
        s_body
    ))
    story.append(Spacer(1, 3))
    story.append(Paragraph(
        "<b>Criterio de finalización alcanzado:</b> Todas las instrucciones de la máquina virtual (MOV, operaciones aritméticas, "
        "lógicas, desplazamientos y comparaciones) operan correctamente sobre registros sectorizados (AL/AH/AX/EAX..FL/FH/FX/EFX) "
        "y sobre accesos a memoria de 1, 2 y 4 bytes. Se respeta estrictamente el truncado en escrituras, la extensión de signo "
        "en lecturas y la preservación de los bytes no involucrados en el registro destino.",
        s_body
    ))
    story.append(Spacer(1, 6))

    # ------------------ 2. CODIFICACIÓN DE OPERANDOS ------------------
    story.append(Paragraph("2. Especificación Técnica de Registros y Operandos MV2", s_h1))
    
    story.append(Paragraph("2.1. Banco Completo de 32 Registros (include/registros.h)", s_h2))
    story.append(Paragraph(
        "El procesador dispone de 32 registros de 32 bits, categorizados según su rol arquitectónico:",
        s_body
    ))
    
    reg_table_data = [
        [
            Paragraph("<b>Código</b>", s_table_hdr), Paragraph("<b>Nombre</b>", s_table_hdr), Paragraph("<b>Función / Descripción</b>", s_table_hdr),
            Paragraph("<b>Código</b>", s_table_hdr), Paragraph("<b>Nombre</b>", s_table_hdr), Paragraph("<b>Función / Descripción</b>", s_table_hdr)
        ],
        [Paragraph("0", s_table_cell_center), Paragraph("IP", s_table_cell_bold), Paragraph("Instruction Pointer (puntero de instrucción)", s_table_cell),
         Paragraph("16", s_table_cell_center), Paragraph("AC", s_table_cell_bold), Paragraph("Acumulador (resto de DIV)", s_table_cell)],
        [Paragraph("1", s_table_cell_center), Paragraph("OPC", s_table_cell_bold), Paragraph("Código de operación actual", s_table_cell),
         Paragraph("17", s_table_cell_center), Paragraph("CC", s_table_cell_bold), Paragraph("Código de condición (Flags N, Z, C, V)", s_table_cell)],
        [Paragraph("2", s_table_cell_center), Paragraph("OP1", s_table_cell_bold), Paragraph("Registro de operando 1 (tipo + bytes)", s_table_cell),
         Paragraph("18..25", s_table_cell_center), Paragraph("-", s_table_cell), Paragraph("Reservados para expansión futura", s_table_cell)],
        [Paragraph("3", s_table_cell_center), Paragraph("OP2", s_table_cell_bold), Paragraph("Registro de operando 2 (tipo + bytes)", s_table_cell),
         Paragraph("26", s_table_cell_center), Paragraph("CS", s_table_cell_bold), Paragraph("Code Segment pointer (16b seg, 16b off)", s_table_cell)],
        [Paragraph("4", s_table_cell_center), Paragraph("LAR", s_table_cell_bold), Paragraph("Logic Address Register (dir lógica)", s_table_cell),
         Paragraph("27", s_table_cell_center), Paragraph("DS", s_table_cell_bold), Paragraph("Data Segment pointer (-1 si tamaño 0)", s_table_cell)],
        [Paragraph("5", s_table_cell_center), Paragraph("MAR", s_table_cell_bold), Paragraph("Memory Address Reg (16b cant, 16b f)", s_table_cell),
         Paragraph("28", s_table_cell_center), Paragraph("ES", s_table_cell_bold), Paragraph("Extra Segment pointer (-1 si tamaño 0)", s_table_cell)],
        [Paragraph("6", s_table_cell_center), Paragraph("MBR", s_table_cell_bold), Paragraph("Memory Buffer Register (dato leido/escrito)", s_table_cell),
         Paragraph("29", s_table_cell_center), Paragraph("SS", s_table_cell_bold), Paragraph("Stack Segment pointer", s_table_cell)],
        [Paragraph("7", s_table_cell_center), Paragraph("SP", s_table_cell_bold), Paragraph("Stack Pointer (tope: SS + stack_size)", s_table_cell),
         Paragraph("30", s_table_cell_center), Paragraph("KS", s_table_cell_bold), Paragraph("Const Segment pointer (-1 si tamaño 0)", s_table_cell)],
        [Paragraph("8", s_table_cell_center), Paragraph("BP", s_table_cell_bold), Paragraph("Base Pointer (marco de pila)", s_table_cell),
         Paragraph("31", s_table_cell_center), Paragraph("PS", s_table_cell_bold), Paragraph("Param Segment pointer (0 si -p, sino -1)", s_table_cell)],
        [Paragraph("10..15", s_table_cell_center), Paragraph("EAX..EFX", s_table_cell_bold), Paragraph("Propósito general (soportan sectores)", s_table_cell),
         Paragraph("9", s_table_cell_center), Paragraph("-", s_table_cell), Paragraph("Reservado", s_table_cell)]
    ]
    t_reg = Table(reg_table_data, colWidths=[32, 45, 185, 38, 45, 178])
    t_reg.setStyle(TableStyle([
        ('BACKGROUND', (0,0), (-1,0), c_primary),
        ('BOX', (0,0), (-1,-1), 0.5, c_border),
        ('INNERGRID', (0,0), (-1,-1), 0.5, c_border),
        ('VALIGN', (0,0), (-1,-1), 'MIDDLE'),
        ('TOPPADDING', (0,0), (-1,-1), 1.5),
        ('BOTTOMPADDING', (0,0), (-1,-1), 1.5),
        ('LEFTPADDING', (0,0), (-1,-1), 4),
        ('RIGHTPADDING', (0,0), (-1,-1), 4),
    ]))
    story.append(t_reg)
    story.append(Spacer(1, 5))

    story.append(Paragraph("2.2. Codificación de Operandos de Registro (Sectores)", s_h2))
    story.append(Paragraph(
        "En un operando de registro (1 byte en la instrucción): bit 7 reservado (0), bits 6-5 indican el sector, y bits 4-0 el código de registro (0..31):",
        s_body
    ))
    
    sec_data = [
        [Paragraph("<b>Código Binario (bits 6-5)</b>", s_table_hdr), Paragraph("<b>Descripción</b>", s_table_hdr), Paragraph("<b>Tamaño</b>", s_table_hdr), Paragraph("<b>Ejemplo (EAX)</b>", s_table_hdr), Paragraph("<b>Comportamiento de Escritura y Lectura</b>", s_table_hdr)],
        [Paragraph("00", s_table_cell_center), Paragraph("Registro completo", s_table_cell), Paragraph("4 bytes", s_table_cell_center), Paragraph("EAX", s_table_cell_bold), Paragraph("Acceso a los 32 bits completos del registro.", s_table_cell)],
        [Paragraph("01", s_table_cell_center), Paragraph("4to byte del registro (byte bajo)", s_table_cell), Paragraph("1 byte", s_table_cell_center), Paragraph("AL", s_table_cell_bold), Paragraph("Modifica bits 7..0; preserva bits 31..8 intactos. Lectura con extensión de signo a 32 bits.", s_table_cell)],
        [Paragraph("10", s_table_cell_center), Paragraph("3er byte del registro (byte alto)", s_table_cell), Paragraph("1 byte", s_table_cell_center), Paragraph("AH", s_table_cell_bold), Paragraph("Modifica bits 15..8; preserva bits 31..16 y 7..0. Lectura con extensión de signo a 32 bits.", s_table_cell)],
        [Paragraph("11", s_table_cell_center), Paragraph("Registro de 2 bytes (word bajo)", s_table_cell), Paragraph("2 bytes", s_table_cell_center), Paragraph("AX", s_table_cell_bold), Paragraph("Modifica bits 15..0; preserva bits 31..16 intactos. Lectura con extensión de signo a 32 bits.", s_table_cell)]
    ]
    t_sec = Table(sec_data, colWidths=[100, 115, 55, 65, 188])
    t_sec.setStyle(TableStyle([
        ('BACKGROUND', (0,0), (-1,0), c_primary),
        ('BOX', (0,0), (-1,-1), 0.5, c_border),
        ('INNERGRID', (0,0), (-1,-1), 0.5, c_border),
        ('VALIGN', (0,0), (-1,-1), 'MIDDLE'),
        ('TOPPADDING', (0,0), (-1,-1), 2),
        ('BOTTOMPADDING', (0,0), (-1,-1), 2),
        ('LEFTPADDING', (0,0), (-1,-1), 4),
        ('RIGHTPADDING', (0,0), (-1,-1), 4),
    ]))
    story.append(t_sec)
    story.append(Spacer(1, 5))

    story.append(Paragraph("2.3. Codificación de Operandos de Memoria (Modificadores b[], w[], l[])", s_h2))
    story.append(Paragraph(
        "En un operando de memoria (3 bytes en la instrucción): los primeros 2 bytes representan el desplazamiento con signo (int16_t en big-endian). "
        "En el tercer byte, los 3 bits más significativos (bits 7-5) definen la cantidad de bytes a transferir, y los 5 bits menos significativos (bits 4-0) "
        "el registro base lógico:",
        s_body
    ))

    mem_data = [
        [Paragraph("<b>Binario (bits 7-5)</b>", s_table_hdr), Paragraph("<b>Modificador</b>", s_table_hdr), Paragraph("<b>Tamaño</b>", s_table_hdr), Paragraph("<b>Ejemplo Assembler</b>", s_table_hdr), Paragraph("<b>Detalle de Transferencia de Memoria</b>", s_table_hdr)],
        [Paragraph("001", s_table_cell_center), Paragraph("b (byte)", s_table_cell), Paragraph("1 byte", s_table_cell_center), Paragraph("MOV b[DS+0], AH", s_table_cell_bold), Paragraph("Accede a 1 byte en RAM. MAR parte alta = 1. MBR = 1 byte. Lectura extiende signo.", s_table_cell)],
        [Paragraph("010", s_table_cell_center), Paragraph("w (word)", s_table_cell), Paragraph("2 bytes", s_table_cell_center), Paragraph("MOV w[EDX+0], CX", s_table_cell_bold), Paragraph("Accede a 2 bytes en big-endian. MAR parte alta = 2. MBR = 2 bytes. Lectura extiende signo.", s_table_cell)],
        [Paragraph("100", s_table_cell_center), Paragraph("l (long)", s_table_cell), Paragraph("4 bytes", s_table_cell_center), Paragraph("MOV w[DS+2], l[DS+10]", s_table_cell_bold), Paragraph("Accede a 4 bytes en big-endian. MAR parte alta = 4. MBR = 4 bytes.", s_table_cell)],
        [Paragraph("000", s_table_cell_center), Paragraph("[ ] (por defecto)", s_table_cell), Paragraph("4 bytes", s_table_cell_center), Paragraph("MOV [DS+5], 10", s_table_cell_bold), Paragraph("Modo clásico MV1 retrocompatible. Accede a 4 bytes sin prefijo visible.", s_table_cell)]
    ]
    t_mem = Table(mem_data, colWidths=[80, 85, 55, 115, 188])
    t_mem.setStyle(TableStyle([
        ('BACKGROUND', (0,0), (-1,0), c_primary),
        ('BOX', (0,0), (-1,-1), 0.5, c_border),
        ('INNERGRID', (0,0), (-1,-1), 0.5, c_border),
        ('VALIGN', (0,0), (-1,-1), 'MIDDLE'),
        ('TOPPADDING', (0,0), (-1,-1), 2),
        ('BOTTOMPADDING', (0,0), (-1,-1), 2),
        ('LEFTPADDING', (0,0), (-1,-1), 4),
        ('RIGHTPADDING', (0,0), (-1,-1), 4),
    ]))
    story.append(t_mem)
    story.append(Spacer(1, 8))

    # ------------------ 3. DETALLE DE MODIFICACIONES ------------------
    story.append(Paragraph("3. Detalle de Modificaciones Módulo por Módulo", s_h1))
    
    story.append(Paragraph("<b>3.1. Módulo Decodificador (include/decodificador.h y cpu/decodificador.c)</b>", s_body))
    story.append(Paragraph("• Estructura <code>Operando</code> enriquecida con los campos <code>sector</code>, <code>tamanoMemoria</code> y <code>modifMemoria</code>.", s_bullet))
    story.append(Paragraph("• Definición de constantes claras: <code>SECTOR_COMPLETO</code>, <code>SECTOR_BYTE_BAJO</code>, <code>SECTOR_BYTE_ALTO</code>, <code>SECTOR_WORD_BAJO</code> y modificadores <code>MODIF_MEM_BYTE</code> (001), <code>MODIF_MEM_WORD</code> (010), <code>MODIF_MEM_LONG</code> (100), <code>MODIF_MEM_DEFAULT</code> (000).", s_bullet))
    story.append(Paragraph("• <code>decodificarPrimerByte</code>: Reconoce <code>OPCODE_STOP</code> (0x0F) y <code>OPCODE_RET</code> (0x0E) como <code>SIN_OPERANDOS</code>, y <code>OPCODE_PUSH</code> (0x0B), <code>OPCODE_POP</code> (0x0C), <code>OPCODE_CALL</code> (0x0D) como <code>UN_OPERANDO</code>.", s_bullet))
    story.append(Paragraph("• <code>leerOperando</code>: Extrae limpiamente los 2 bits de sector de los registros y los 3 bits de modificador de memoria con mapeo robusto a su tamaño en bytes.", s_bullet))
    story.append(Paragraph("• Implementación de <code>obtenerTamanoOperando(Operando o)</code> y <code>truncarYExtenderSigno(uint32_t val, int tam)</code>.", s_bullet))
    story.append(Spacer(1, 3))

    story.append(Paragraph("<b>3.2. Módulo de CPU y Ejecución (include/cpu.h y cpu/cpu.c)</b>", s_body))
    story.append(Paragraph("• <code>calcularDirLogica</code>: Valida segmentos no asignados (0xFFFFFFFF) y accesos con desborde fuera del rango de 16 bits.", s_bullet))
    story.append(Paragraph("• <code>leerValorOperando</code>: Soporta lectura de sectores (AL, AH, AX) y memoria de 1 o 2 bytes con <b>propagación de signo</b> a 32 bits, respetando la especificación oficial.", s_bullet))
    story.append(Paragraph("• <code>escribirValorOperando</code>: En escrituras a registros (AL, AH, AX), aplica máscaras bit a bit para actualizar únicamente los bytes del sector correspondiente, preservando el resto de los bits del registro de 32 bits. En memoria, delega con el tamaño exacto.", s_bullet))
    story.append(Paragraph("• <code>hayMasInstrucciones</code>: Adaptado para procesos MV2 comparando el segmento de IP contra <code>r[REGCS] &gt;&gt; 16</code>, garantizando funcionamiento con cualquier orden de segmentos.", s_bullet))
    story.append(Paragraph("• <code>ejecutarInstruccion</code>: Actualización de <code>MOV</code> calculando flags CC sobre el valor efectivo del destino, y <code>SWAP</code> directo entre operandos.", s_bullet))
    story.append(Spacer(1, 3))

    story.append(Paragraph("<b>3.3. Módulo de Memoria Principal (memoria/memoria.c)</b>", s_body))
    story.append(Paragraph("• <code>escribirEnMemoria</code>: Almacena en <code>r[REGMBR]</code> el dato exacto transferido (1 byte para b[], 2 bytes para w[] y 4 bytes para l[]), reflejando la simetría con <code>leerDeMemoria</code>.", s_bullet))
    story.append(Spacer(1, 3))

    story.append(Paragraph("<b>3.4. Módulo Disassembler (io/disassembler.c)</b>", s_body))
    story.append(Paragraph("• <code>nombreRegistro</code>: Incorporación de los registros SP (7), BP (8), ES (28), SS (29), KS (30) y PS (31).", s_bullet))
    story.append(Paragraph("• <code>formatearOperando</code>: Visualización de pseudónimos AL/AH/AX..FL/FH/FX para EAX..EFX y prefijos b, w, l para memoria, preservando el formato sin prefijo para binarios MV1.", s_bullet))
    story.append(Paragraph("• <code>mnemonico</code>: Soporte para los nuevos códigos PUSH, POP, CALL y RET.", s_bullet))
    story.append(Spacer(1, 8))

    # ------------------ 4. RESULTADOS DE PRUEBAS ------------------
    story.append(Paragraph("4. Batería de Pruebas y Resultados de Validación", s_h1))
    
    test_data = [
        [Paragraph("<b>ID</b>", s_table_hdr), Paragraph("<b>Comando / Suite de Prueba</b>", s_table_hdr), Paragraph("<b>Comportamiento Verificado</b>", s_table_hdr), Paragraph("<b>Resultado</b>", s_table_hdr)],
        [Paragraph("REG-01", s_table_cell_center), Paragraph("vmx tests\\sample.vmx", s_table_cell), Paragraph("Ejecución completa de programa aritmético/lógico MV1 con salida idéntica.", s_table_cell), Paragraph("PASÓ (OK)", s_table_cell_ok)],
        [Paragraph("REG-02", s_table_cell_center), Paragraph("vmx tests\\sample.vmx -d", s_table_cell), Paragraph("Desensamblado MV1 con formato y alineación conservados sin regresiones.", s_table_cell), Paragraph("PASÓ (OK)", s_table_cell_ok)],
        [Paragraph("REG-03", s_table_cell_center), Paragraph("vmx tests\\sample2.vmx", s_table_cell), Paragraph("Operaciones de salto y límites enteros en MV1 con resultados idénticos.", s_table_cell), Paragraph("PASÓ (OK)", s_table_cell_ok)],
        [Paragraph("ET3-REG", s_table_cell_center), Paragraph("tests\\test_etapa3.exe", s_table_cell), Paragraph("Suite completa de Etapa 3 (RAM variable, segmentos contiguos y tabla).", s_table_cell), Paragraph("PASÓ (OK)", s_table_cell_ok)],
        [Paragraph("ET4-01", s_table_cell_center), Paragraph("test_etapa4: Sectores Registro", s_table_cell), Paragraph("Decodificación de sectores 00 (completo), 01 (bajo), 10 (alto), 11 (word) en EAX..EFX.", s_table_cell), Paragraph("PASÓ (OK)", s_table_cell_ok)],
        [Paragraph("ET4-02", s_table_cell_center), Paragraph("test_etapa4: Modificadores Mem", s_table_cell), Paragraph("Decodificación de modificadores b[] (1B), w[] (2B), l[] (4B) y [] por defecto (4B).", s_table_cell), Paragraph("PASÓ (OK)", s_table_cell_ok)],
        [Paragraph("ET4-03", s_table_cell_center), Paragraph("test_etapa4: Registros SP..PS", s_table_cell), Paragraph("Comprobación de códigos 7, 8, 26, 27, 28, 29, 30, 31 y lectura/escritura.", s_table_cell), Paragraph("PASÓ (OK)", s_table_cell_ok)],
        [Paragraph("ET4-04a", s_table_cell_center), Paragraph("test_etapa4: MOV AL, inmed", s_table_cell), Paragraph("Prueba mínima: Preserva bits 31..8, trunca a 8 bits y extiende signo en lectura.", s_table_cell), Paragraph("PASÓ (OK)", s_table_cell_ok)],
        [Paragraph("ET4-04b", s_table_cell_center), Paragraph("test_etapa4: MOV AX, inmed", s_table_cell), Paragraph("Prueba mínima: Preserva bits 31..16, trunca a 16 bits y extiende signo en lectura.", s_table_cell), Paragraph("PASÓ (OK)", s_table_cell_ok)],
        [Paragraph("ET4-04c", s_table_cell_center), Paragraph("test_etapa4: MOV b[dir], AH", s_table_cell), Paragraph("Prueba mínima: Extrae AH, escribe 1 byte en RAM, valida LAR, MAR (1B) y MBR.", s_table_cell), Paragraph("PASÓ (OK)", s_table_cell_ok)],
        [Paragraph("ET4-04d", s_table_cell_center), Paragraph("test_etapa4: MOV w[dir], CX", s_table_cell), Paragraph("Prueba mínima: Escribe 2 bytes en big-endian, valida LAR, MAR (2B) y MBR.", s_table_cell), Paragraph("PASÓ (OK)", s_table_cell_ok)],
        [Paragraph("ET4-04e", s_table_cell_center), Paragraph("test_etapa4: Lectura/escritura l[dir]", s_table_cell), Paragraph("Prueba mínima: Transferencia de 4 bytes en memoria, valida MAR (4B) y MBR.", s_table_cell), Paragraph("PASÓ (OK)", s_table_cell_ok)],
        [Paragraph("ET4-05", s_table_cell_center), Paragraph("test_etapa4: SHL EL, 1", s_table_cell), Paragraph("Ejemplo de ASM MV2: Desplaza 8 bits bajos de EEX preservando los 24 bits altos.", s_table_cell), Paragraph("PASÓ (OK)", s_table_cell_ok)],
        [Paragraph("ET4-06", s_table_cell_center), Paragraph("test_etapa4: MOV w[2], l[10]", s_table_cell), Paragraph("Ejemplo de ASM MV2: Copia byte 12 al 2 y byte 13 al 3 del Data Segment.", s_table_cell), Paragraph("PASÓ (OK)", s_table_cell_ok)],
        [Paragraph("ET4-07", s_table_cell_center), Paragraph("test_etapa4: SWAP con sectores", s_table_cell), Paragraph("Intercambio de registros con sectores preservando bits no afectados.", s_table_cell), Paragraph("PASÓ (OK)", s_table_cell_ok)],
        [Paragraph("ET4-08", s_table_cell_center), Paragraph("test_etapa4: Pipeline Binario", s_table_cell), Paragraph("Fetch, decodificación binaria, desensamblado y ejecución completa con STOP.", s_table_cell), Paragraph("PASÓ (OK)", s_table_cell_ok)]
    ]
    t_test = Table(test_data, colWidths=[42, 125, 290, 66])
    t_test.setStyle(TableStyle([
        ('BACKGROUND', (0,0), (-1,0), c_primary),
        ('BOX', (0,0), (-1,-1), 0.5, c_border),
        ('INNERGRID', (0,0), (-1,-1), 0.5, c_border),
        ('VALIGN', (0,0), (-1,-1), 'MIDDLE'),
        ('TOPPADDING', (0,0), (-1,-1), 2),
        ('BOTTOMPADDING', (0,0), (-1,-1), 2),
        ('LEFTPADDING', (0,0), (-1,-1), 4),
        ('RIGHTPADDING', (0,0), (-1,-1), 4),
    ]))
    story.append(t_test)
    story.append(Spacer(1, 8))

    # ------------------ 5. CONCLUSIÓN ------------------
    story.append(Paragraph("5. Conclusión y Próximos Pasos (Etapa 5)", s_h1))
    story.append(Paragraph(
        "La <b>Etapa 4</b> ha sido completada con éxito absoluto. El procesador y el decodificador ahora interpretan "
        "sin ambigüedades cualquier combinación de registros, sectores (AL/AH/AX/EAX..), tamaños de memoria (b/w/l/[]) e inmediatos. "
        "La compilación con <code>-Wall -Wextra</code> se mantiene limpia (0 errores, 0 warnings) y todas las regresiones "
        "de la Versión 1 y Etapa 3 continúan aprobando al 100%.",
        s_body
    ))
    story.append(Spacer(1, 3))
    story.append(Paragraph(
        "<b>Conexión directa con la Etapa 5:</b> Con los registros SP, BP y SS disponibles y el decodificador habilitado "
        "para decodificar operandos de tamaño variable con truncado y extensión de signo, el proyecto se encuentra en estado "
        "óptimo para encarar la <b>Etapa 5: Pila e instrucciones PUSH / POP / CALL / RET</b>.",
        s_body
    ))

    doc.build(story, canvasmaker=NumberedCanvas)
    print(f"Informe generado con éxito en: {ruta_salida}")

if __name__ == "__main__":
    salida = os.path.abspath("Documentacion/Informe_Etapa_4_Registros_y_Operandos_MV2.pdf")
    generar_pdf(salida)
