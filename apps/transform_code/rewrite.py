variables_est=[
    "uint8_t LD_I1 = 0;\n"
    ,"uint8_t LD_I2 = 0;\n"
    ,"uint8_t LD_I3 = 0;\n"
    ,"uint8_t LD_I4 = 0;\n"
    ,"uint8_t LD_I5 = 0;\n"
    ,"uint8_t LD_I6 = 0;\n"
    ,"uint8_t LD_Q1 = 0;\n"
    ,"uint8_t LD_Q2 = 0;\n"
    ,"uint8_t LD_Q3 = 0;\n"
    ,"uint8_t LD_Q4 = 0;\n"
]

def delete_repeat_variables_est(cont_archivo_init):
    i_start = 0
    i_end = 0
    contador=0
    lineas = cont_archivo_init

    for var in variables_est:

        for i,linea in enumerate(lineas):

            if var in linea:
                lineas[i] = ""
                break
    return lineas

def get_content_list(nombre_archivo_init,in_inst,out_inst):
    try:
        with open(nombre_archivo_init,'r') as archivo_init:
            i_start = 0
            i_end = 0
            lineas = archivo_init.readlines()
            for i, linea in enumerate(lineas):
                if in_inst in linea:
                    i_start = i
                if out_inst in linea:
                    i_end = i
            contenido_init = lineas[i_start+1:i_end]
            return contenido_init
    except FileNotFoundError:
        print("El archivo original no fue encontrado.")


def mod(content):
    i_start = 0
    i_end =0
    for i,linea in enumerate(content):
        if "now = millis();" in linea:
            content[i] = "unsigned long now = esp_timer_get_time()/1000;"
    i_start = 0
    i_end =0
    #change inputs
    for i,linea in enumerate(content):
        if "void readInputs()" in linea:
            i_start = i
        if "}\n" == linea and i_start != 0:
            i_end = i
            break
    newInputsContent = get_content_list("template.txt","/readInputs/","/readInputs-end/")

    content = content[:i_start+1] + newInputsContent +content[i_end:]

    i_start = 0
    i_end =0
    #change outpus
    for i,linea in enumerate(content):
        if "void writeOutputs()" in linea:
            i_start = i
        if "}\n" == linea and i_start != 0:
            i_end = i
            break
    newOutputsContent = get_content_list("template.txt","/writeOutputs/","/writeOutputs-end/")

    content = content[:i_start+1] + newOutputsContent +content[i_end:]

    i_start = 0
    i_end =0
    #change init 
    for i,linea in enumerate(content):
        if "void init()" in linea:
            i_start = i
        if "}\n" == linea and i_start != 0:
            i_end = i
            break
    newContent = get_content_list("template.txt","/voidinit/","/voidinit-end/")

    content = content[:i_start+1] + newContent +content[i_end:]

    i_start = 0
    i_end =0
    for i,linea in enumerate(content):
        if "void TaskScan(void *pvParameters){" in linea:
            i_start = i
        if "}\n" == linea and i_start != 0:
            i_end = i+1 #porque tiene un for adentro de la funccion y tiene cierre de llaves antes
            break
    newContent = ["void app(){\n","   init();\n","    initContext();\n"]+content[i_start+1:i_end]

    content = content[:i_start] + content[i_end:]

    i_start = 0
    i_end =0
    for i,linea in enumerate(content):
        if "void setup()" in linea:
            i_start = i
        if "}\n" == linea and i_start != 0:
            i_end = i
            break

    content = content[:i_start] + newContent +content[i_end+1:]

    i_start = 0
    i_end =0
    for i,linea in enumerate(content):
        if "void loop()" in linea:
            i_start = i
        if "}\n" == linea and i_start != 0:
            i_end = i
            break

    content = content[:i_start] +content[i_end+1:]

    return content




def modificar_archivo(nombre_archivo_original,nombre_archivo_modificado):
    try:
        with open(nombre_archivo_original,'r') as archivo_original:
            lineas = archivo_original.readlines()

            for i,linea in enumerate(lineas):
                if "typedef struct" in linea:
                    cont = lineas[i:]
                    break
                elif "union {" in linea:
                    cont = lineas[i:]
                    break

    except FileNotFoundError:
        print("El archivo original no fue encontrado.")

    #defino librerias, entradas, salidas y var_est.
    cont = delete_repeat_variables_est(cont)
    init = get_content_list("template.txt","/init/","/init-end/") + cont
    #delete repeat state vars definitions, or delete repeat code lines
    #a = delete_repeat_variables_est(init)
    final = mod(init)
    result_file = open("plc.c",'w')
    result_file.writelines(final)
    result_file.close()

    





def main():
    import argparse
    parser = argparse.ArgumentParser(description="pasar codigo de arduino(esp32_plc) a c")
    # Nombres de los archivos
    parser.add_argument("archivo_original", help="Nombre del archivo ")
    parser.add_argument("archivo_modificado", help="Nombre del archivo modificado.")
    args = parser.parse_args()




    modificar_archivo(args.archivo_original, args.archivo_modificado)

if __name__ == "__main__":
    main()