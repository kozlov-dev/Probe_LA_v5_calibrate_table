dacValA_m12=[]
dacValB_m12=[]

dacValA_m27=[]
dacValB_m27=[]

table = {'V12': [], 'V27': []}

input_path = "C:/Users/kozlov/STM32CubeIDE/workspace/Probe_LA_v5_calibrate_table/table/logic_calibration_table.txt"
output_path = "C:/Users/kozlov/STM32CubeIDE/workspace/Probe_LA_v5_calibrate_table/Core/Src/logic_calibration_table1.c"

output_path2 = "C:/Users/kozlov/STM32CubeIDE/workspace/Probe_LA_v5_calibrate_table/Core/Inc/logic_calibration_table1.h"

with open(input_path) as f:
    key = ''
    for line in f:
        if line.find('//12V') != -1:
            key = 'V12'
        elif line.find('//27V') != -1:
            key = 'V27'
        else:
            table[key].append(list(map(int, line.split())))
    print(table)



for i in range(len(list(table["V12"]))):
    print(table["V12"][i][0]) # V
    dacValA_m12.append(table["V12"][i][1])
    # print(table["V12"][i][1]) # dacValA
    dacValB_m12.append(table["V12"][i][2])
    # print(table["V12"][i][2]) # dacValB
    
for i in range(len(list(table["V27"]))):
    # print(table["V27"][i][0]) # V
    # print(table["V27"][i][1]) # dacValA
    dacValA_m27.append(table["V27"][i][1])
    # print(table["V27"][i][2]) # dacValB
    dacValB_m27.append(table["V27"][i][2])








# Запись файла 
f = open(output_path, "w")


f.write("/*******************************************************************************\n")
f.write(" * @File: logic_calibration_table.c\n")
f.write(" * @Author: Kozlov-dev, A.\n")
f.write(" * @Project: Probe_LA.c")
f.write(" * @Microcontroller: STM32F103C8T\n")
f.write(" * @Device: Probe_LA\n")
f.write(" * @Date: 07.04.2021\n")
f.write(" * @Purpose: Calibration table volt2dac\n")
f.write(" *******************************************************************************/\n")


EOF = "\n"
f.write("     void crete_calibration_table(Table_t *calibTable){"+EOF)

for item in list(range(len(dacValA_m12))):
    f.write("      calibTable->" + "dacValA_m12["+str(item)+"] = "+"%s;\n" % hex(dacValA_m12[item]))
f.write("\n")

for item in list(range(len(dacValB_m12))):
    f.write("     calibTable->" + "dacValB_m12["+str(item)+"] = "+"%s;\n" % hex(dacValB_m12[item]))
f.write("\n")

for item in list(range(len(dacValA_m27))):
    f.write("     calibTable->" + "dacValA_m27["+str(item)+"] = "+"%s;\n" % hex(dacValA_m27[item]))
f.write("\n")

for item in list(range(len(dacValB_m27))):
    f.write("     calibTable->" + "dacValB_m27["+str(item)+"] = "+"%s;\n" % hex(dacValB_m27[item]))
f.write("\n")



f.write(" }"+EOF)

f.close()

#-------------------------------------------------------------------------------------------
f = open(output_path2, "w")
f.write("/*******************************************************************************\n")
f.write(" * @File: logic_calibration_table.c\n")
f.write(" * @Author: Kozlov-dev, A.\n")
f.write(" * @Project: Probe_LA.c")
f.write(" * @Microcontroller: STM32F103C8T\n")
f.write(" * @Device: Probe_LA\n")
f.write(" * @Date: 07.04.2021\n")
f.write(" * @Purpose: Calibration table volt2dac\n")
f.write(" *******************************************************************************/\n")


step = -(int(table["V12"][1][0]) - int(table["V12"][2][0]))
MAX_VOLT_MODE_12 = table["V12"][len(table["V12"])-1][0]
print(len(table["V12"]))
     

EOF = "\n"

f.write("#ifndef LOGIC_CALIBRATION_TABLE_H" +EOF)
f.write("#ifndef LOGIC_CALIBRATION_TABLE_H"+EOF)
f.write(EOF)
f.write("#define MAX_VAL_M12            "+ str(len(dacValA_m12))+EOF)
f.write("#define MAX_VAL_M27            "+ str(len(dacValA_m27))+EOF)
f.write(EOF)
f.write("#define STEP_CALIBRATE         "+ str(step)+EOF)
f.write(EOF)
f.write("#define MIN_VOLT_MODE_12         "+ str(table["V12"][0][0])+EOF)
f.write("#define MAX_VOLT_MODE_12         "+ str(MAX_VOLT_MODE_12)+EOF)

f.write("#define MIN_VOLT_MODE_27         "+ str(table["V27"][0][0])+EOF)
f.write("#define MAX_VOLT_MODE_27         "+ str(table["V27"][1][0])+EOF)


f.write("\n")






















# f.write("#define MAX_VAL_M12 88" +EOF)
# f.write("#define MAX_VAL_M27 163"+EOF)

# f.write("#define MAGIC_KEY_DEFINE 0x4815162342"+EOF)

# f.write("typedef struct "+EOF)
# f.write("{"+EOF)

# f.write("    uint32_t MagicNum; //0x4815162342"+EOF)
# f.write("    uint16_t dacValA_m12[MAX_VAL_M12];"+EOF)
# f.write("    uint16_t dacValB_m12[MAX_VAL_M12];"+EOF)
# f.write("    uint16_t dacValA_m27[MAX_VAL_M27];"+EOF)
# f.write("    uint16_t dacValB_m27[MAX_VAL_M27];"+EOF)
# f.write("}table_t;"+EOF)

# f.write("struct FLASH_Sector"+EOF)
# f.write("{")
 
# f.write("    uint16_t data [2048-8];"+EOF)
# f.write("    uint32_t NWrite;"+EOF)
# f.write("    uint32_t CheckSum;"+EOF)
# f.write("};"+EOF)
# f.write(" union NVRAM"+EOF)
# f.write(" {"+EOF)

# f.write("     struct FLASH_Sector sector;"+EOF)
# f.write("     uint16_t data16[678];"+EOF)
# f.write(" }DevNVRAM;"+EOF)
# f.write(""+EOF)
# f.write(""+EOF)
# f.write(" // Чтение данных из FLASH памяти"+EOF)
# f.write(" // Если после чтения мейджик-кей не найден, то это первый запуск"+EOF)
# f.write(" if (DevNVRAM.calibration_table.MagicNum != MAGIC_KEY_DEFINE)"+EOF)
# f.write(" {"+EOF)

# f.write("     Очистка даты и запись дефолтных значений"+EOF)

# 1-88
# for item in list(range(len(dacValA_m12))):
#     f.write("     DevNVRAM->calibration_table." + "dacValA_m12["+str(item)+"] = "+"%s;\n" % hex(item))
# f.write("\n")

# for item in list(range(len(dacValB_m12))):
#     f.write("     DevNVRAM->calibration_table." + "dacValB_m12["+str(item)+"] = "+"%s;\n" % hex(item))
# f.write("\n")

# for item in list(range(len(dacValA_m27))):
#     f.write("     DevNVRAM->calibration_table." + "dacValA_m27["+str(item)+"] = "+"%s;\n" % hex(item))

# for item in list(range(len(dacValB_m27))):
#     f.write("     DevNVRAM->calibration_table." + "dacValB_m27["+str(item)+"] = "+"%s;\n" % hex(item))
# f.write("\n")


# for item in list(range(len(dacValA_m12))):
#     f.write("     DevNVRAM->calibration_table." + "dacValA_m12["+str(item)+"] = "+"%s;\n" % hex(dacValA_m12[item]))
# f.write("\n")

# for item in list(range(len(dacValB_m12))):
#     f.write("     DevNVRAM->calibration_table." + "dacValB_m12["+str(item)+"] = "+"%s;\n" % hex(dacValB_m12[item]))
# f.write("\n")

# for item in list(range(len(dacValA_m27))):
#     f.write("     DevNVRAM->calibration_table." + "dacValA_m27["+str(item)+"] = "+"%s;\n" % hex(dacValA_m27[item]))
# f.write("\n")

# for item in list(range(len(dacValB_m27))):
#     f.write("     DevNVRAM->calibration_table." + "dacValB_m27["+str(item)+"] = "+"%s;\n" % hex(dacValB_m27[item]))
# f.write("\n")




# for item in list(range(len(dacValA_m12))):
#     f.write("     DevNVRAM.calibration_table." + "dacValA_m12["+str(item)+"] = "+"%s;\n" % hex(dacValA_m12[item]))
# f.write("\n")

# for item in list(range(len(dacValB_m12))):
#     f.write("     DevNVRAM.calibration_table." + "dacValB_m12["+str(item)+"] = "+"%s;\n" % hex(dacValB_m12[item]))
# f.write("\n")

# for item in list(range(len(dacValA_m27))):
#     f.write("     DevNVRAM.calibration_table." + "dacValA_m27["+str(item)+"] = "+"%s;\n" % hex(dacValA_m27[item]))
# f.write("\n")

# for item in list(range(len(dacValB_m27))):
#     f.write("     DevNVRAM.calibration_table." + "dacValB_m27["+str(item)+"] = "+"%s;\n" % hex(dacValB_m27[item]))
# f.write("\n")

