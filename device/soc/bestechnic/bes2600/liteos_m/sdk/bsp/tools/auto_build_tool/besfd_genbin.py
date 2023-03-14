import os
import sys
import time

# argv: distdir best2003 16m_flash|32m_flash sec|nosec
# get external parameters
argv_list = []
for i in range(len(sys.argv)):
    if i == 0:
        continue
    argv_list.append(sys.argv[i])

# M_flash is initialized to the '16m_flash' character default, representing best2003 16m_flash
M_flash = '16m_flash'
chip = 'best2003'

# external parameter format judgment
external_para_format = 'External parameter format wrong.'


if len(argv_list) < 1:
    raise RuntimeError(external_para_format)

distdir=argv_list[0]

if len(argv_list) > 1:
    chip=argv_list[1]
    if chip != 'best2001' and chip != 'best2003':
        raise RuntimeError(external_para_format)

if len(argv_list) > 2:
    M_flash = argv_list[2]
    if chip != 'best2003':
        raise RuntimeError(external_para_format)
    elif M_flash != '16m_flash' and M_flash != '32m_flash':
        raise RuntimeError(external_para_format)


# read config file
#open file, then operate , final, f_cfg.close()
f_cfg = open('bes_sign_cfg.yaml')


# keep all files
cfg_dict = {}
# name list of the four categories SIGN_KEY, PUB_OTP etc.
dict1_name_list = []
# keep EXTRACT_CMD
dict2_name_list = []
# count the four categories SIGN_KEY, PUB_OTP etc.
num_of_first_items = -1
# count the EXTRACT_CMD
num_of_second_items = -1
is_list = False
# count the SIGN_FILE_OUT_LIST
file_num_out_list = -1
# count the SIGN_FILE_LIST
file_num_list = -1

while True:
    line = f_cfg.readline()
    if not line:
        break

    # Divide the line into two paragraphs, starting with the first "~"
    arry_parm = line.split('~', 1 )

    if len(arry_parm) == 2:
        is_list = True
    else :
        arry_parm = line.split(':', 1 )
        is_list = False


    para_key = ''
    para_val = ''
    if len(arry_parm) == 2:
        para_key = arry_parm[0]
        para_val = arry_parm[1]

    # four categories,and init dict_values
    if para_key[0:1].isspace() == False:
        num_of_first_items = num_of_first_items + 1
        if para_val != '\n':
            print("Config format wrong or read end.\n")
            break

        dict1_name_list.append(para_key)

        cfg_dict[dict1_name_list[num_of_first_items]] = {}

        if dict1_name_list[num_of_first_items] == 'SIGN_FILE_OUT_LIST':
            cfg_dict[dict1_name_list[num_of_first_items]] = []

        if dict1_name_list[num_of_first_items] == 'SIGN_FILE_LIST':
            cfg_dict[dict1_name_list[num_of_first_items]] = []

    else:
        # This is a reference assignment
        cfg_tmp = cfg_dict[dict1_name_list[num_of_first_items]]

        # keep SIGN_FILE_OUT_LIST
        if dict1_name_list[num_of_first_items] == 'SIGN_FILE_OUT_LIST':
            if is_list == True and para_val.strip()[-1:] == ':':
                file_num_out_list = file_num_out_list + 1
                cfg_tmp.append({})
                continue

            if para_key.strip() == 'SWITCH':
                cfg_tmp[file_num_out_list][para_key.strip()] = para_val.strip()
                continue

            cfg_tmp[file_num_out_list][para_key.strip()] = para_val.strip()[1:-1]
            continue

        # keep SIGN_FILE_LIST
        if dict1_name_list[num_of_first_items] == 'SIGN_FILE_LIST':
            if is_list == True and para_val.strip()[-1:] == ':':
                file_num_list = file_num_list + 1
                cfg_tmp.append({})
                continue

            if para_key.strip() == 'SWITCH':
                cfg_tmp[file_num_list][para_key.strip()] = para_val.strip()
                continue

            cfg_tmp[file_num_list][para_key.strip()] = para_val.strip()[1:-1]
            continue

        # judge whether it is EXTRACT_CMD
        if para_val == '\n':
            is_list = True
            num_of_second_items = num_of_second_items + 1
            dict2_name_list.append(para_key.strip())
            cfg_tmp[dict2_name_list[num_of_second_items]] = []
            continue

        # keep EXTRACT_CMD
        if is_list == True and para_key[:3].isspace() == True :
            if para_val[0].isspace() == True:
                cfg_tmp[dict2_name_list[num_of_second_items]].append(para_val.strip()[1:-1])
                continue

        # judge SWITCH in BES_KEY
        if str.find(para_key,'SWITCH') != -1:
            cfg_tmp[para_key.strip()] = para_val.strip()
            continue

        cfg_tmp[para_key.strip()] = para_val.strip()[1:-1]

f_cfg.close()
# print(cfg_dict)


# define variables to get dependencies
# BES_KEY
dict_bes_key = cfg_dict['BES_KEY']
rsa_dir_2048 = dict_bes_key['RSA_DIR_2048']
rsa_dir_3072 = dict_bes_key['RSA_DIR_3072']
in_key_dir_2048 = dict_bes_key['IN_KEY_DIR_2048']
in_key_dir_3072 = dict_bes_key['IN_KEY_DIR_3072']
pub_key_name = dict_bes_key['PUB_KEY_NAME']
pri_key_name = dict_bes_key['PRI_KEY_NAME']
sign_cmd_2048 = dict_bes_key['SIGN_CMD_2048']
sign_cmd_3072 = dict_bes_key['SIGN_CMD_3072']
sign_dir = dict_bes_key['SIGN_DIR']
extract_cmd = dict_bes_key['EXTRACT_CMD']
pub_gen_cmd_name_16m_flash = dict_bes_key['PUB_GEN_CMD_NAME_16M_FLASH']
pub_gen_cmd_name_32m_flash = dict_bes_key['PUB_GEN_CMD_NAME_32M_FLASH']
sign_file_dir = dict_bes_key['SIGN_FILE_DIR']
# release_file_dir = dict_bes_key['RELEASE_BIN_DIR']
gui_bin_dir = dict_bes_key['GUI_BIN_DIR']
gui_tools_dir = dict_bes_key['GUI_TOOLS_DIR']
dld_cfg_dir = dict_bes_key['GUI_CFG_YAML_DIR']
dld_cfg_file = dict_bes_key['GUI_CFG_YAML_FILE']
pro_dld_cfg_file = dict_bes_key['PRO_CFG_YAML_FILE']
programmer_name_best2001 = dict_bes_key['PROGRAMMER_NAME_BEST2001']
programmer_name_best2003 = dict_bes_key['PROGRAMMER_NAME_BEST2003']
pack_file_name_best2001 = dict_bes_key['PACK_FILE_NAME_BEST2001']
pack_file_name_best2003_16m_flash = dict_bes_key['PACK_FILE_NAME_BEST2003_16M_FLASH']
pack_file_name_best2003_32m_flash = dict_bes_key['PACK_FILE_NAME_BEST2003_32M_FLASH']


# PUB_OTP
dict_pub_otp = cfg_dict['PUB_OTP']
outkey_dir = dict_pub_otp['OUTKEY_DIR']
outkey_name = dict_pub_otp['OUTKEY_NAME']
pub_otp_name = dict_pub_otp['PUB_OTP_FILE_NAME']

release_file_dir = distdir
# delete the previously generated signature file
if True:
    # ./bes_sign_tool/out_key/outkey.bin
    outkey_bin_dir = sign_dir + '/' + outkey_dir + '/' + outkey_name
    isExists = os.path.exists(outkey_bin_dir)
    if isExists == True:
        os.system('rm -rf ' + outkey_bin_dir)


# path to the integrated signature tool best2001 and best2003
in_key_dir = ''
sign_cmd = ''
rsa_dir = ''
programmer_name = ''
pack_file_name = ''
if True:
    if chip == 'best2001':
        in_key_dir = rsa_dir_2048 + '/' + in_key_dir_2048
        sign_cmd = sign_cmd_2048
        rsa_dir = rsa_dir_2048
        programmer_name = programmer_name_best2001
        pack_file_name = pack_file_name_best2001
    if chip == 'best2003':
        in_key_dir = rsa_dir_3072 + '/' + in_key_dir_3072
        sign_cmd = sign_cmd_3072
        rsa_dir = rsa_dir_3072
        programmer_name = programmer_name_best2003


# get the dir to the private key, public key, outkey.bin, sign_cmd
pri_dir = sign_dir + '/' + in_key_dir + '/' + pri_key_name
pub_dir = sign_dir + '/' + in_key_dir + '/' + pub_key_name
outkey_bin_dir = sign_dir + '/' + outkey_dir + '/' + outkey_name
sign_cmd_dir = sign_dir + '/' + rsa_dir + '/'+ sign_cmd

# get flash command
pub_gen_cmd_name = ' '
if chip == 'best2003':
    if M_flash == '16m_flash':
        pub_gen_cmd_name = pub_gen_cmd_name_16m_flash.replace('pri.pem', pri_dir)
        pack_file_name = pack_file_name_best2003_16m_flash
    if M_flash == '32m_flash':
        pub_gen_cmd_name = pub_gen_cmd_name_32m_flash.replace('pri.pem', pri_dir)
        pack_file_name = pack_file_name_best2003_32m_flash

# check log_switch format
log_switch = True

# chmod sign_cmd
if True:
    print('Start chmoding %s...' % (sign_cmd))
    cmd_list = ''
    cmd_list += 'chmod '
    cmd_list += '777 '
    # Returns the current working directory
    cur_dir = os.getcwd()
    # print(cur_dir)
    cmd_list += cur_dir + '/' + sign_cmd_dir
    if log_switch:
        print(cmd_list)
    result = os.system(cmd_list)
    if result != 0:
        raise RuntimeError('Chmod %s failed.'%(sign_cmd))
    print('Chmod %s done.\n\n'%(sign_cmd))


# #make littlefs
# if True:
#     os.system("echo \"Start make littlefs\"")
#     cmd_list = './bes2300wp_genfs.sh'
#     result = os.system(cmd_list)
#     if result != 0:
#         raise RuntimeError('Make littlefs failed.')
#     print("genfs done. ")
#     # Get the size of the file (in bytes)
#     size = os.path.getsize("../../prebuild/littlefs.bin")
#     print("Littlefs code size:%d"%size)
#     if size<4882432:
#         time.sleep(5)
#     os.system("echo \"Make littlefs done\"")


# extract pub_key and make pub_otp.bin
if True:
    print('Start extracting public key...')
    cmd_list = ''
    for i in range(len(extract_cmd)):
        cmd_list += extract_cmd[i] + ' '
    cur_dir = os.getcwd()
    # print(cur_dir)
    cmd_list += cur_dir + '/' + pub_dir + ' '
    cmd_list += cur_dir + '/' + outkey_bin_dir
    if log_switch:
        print(cmd_list)
    os.chdir(cur_dir + '/' + sign_dir + '/' + outkey_dir)
    # print(os.getcwd())
    result = os.system(cmd_list)
    if result != 0:
        raise RuntimeError('Extract the public key failed.')
    os.chdir(cur_dir)
    print("Extract the public key done.\n\n")


# Make pub_otp
if True:
    print('Start making pub_otp...')
    cmd_list = ''
    #./best_sign -k key/OutKey.bin key/pub_flash2.bin
    cur_dir = os.getcwd()
    cmd_list += sign_cmd_dir + ' '
    if chip == 'best2001':
        cmd_list += '-k' + ' '
    else:
        cmd_list += pub_gen_cmd_name + ' '
    cmd_list += outkey_bin_dir + ' '
    cmd_list += release_file_dir + '/' + pub_otp_name
    if log_switch:
        print(cmd_list,'\n')
    result = os.popen(cmd_list)
    output_terminal = result.read()
    list_line = ''
    for line in output_terminal.split('\n'):
        if len(line) != 0:
            print(line)
            list_line = line
    if list_line != 'Write sec key file done':
        raise RuntimeError(list_line + ". Make %s failed." % (pub_otp_name))
    print("Make %s done.\n\n" % (pub_otp_name))



# define signature function
def bin_sign_fun(dict_sign_file):
    print(dict_sign_file, '\n')
    list_items_file = dict_sign_file
    file_itme_len = len(list_items_file)
    for index in range(file_itme_len):
        file_item = list_items_file[index]
        tmp_file_item = file_item
        if tmp_file_item:
            print(str(file_item))
            sign_file_name = tmp_file_item['REN_BIN_NAME']
            sign_switch = tmp_file_item['SWITCH']
            if sign_switch == 'ON':
                sign_switch = True
            elif sign_switch == 'OFF':
                sign_switch = False
                print('%s sign switch is OFF, no signature required.\n'%(sign_file_name))
            else:
                raise RuntimeError('%s sign switch format wrong,expected ON or OFF.'%(sign_file_name))
            if sign_switch and sign_file_name:
                cmd_list = ''
                cmd_list += sign_cmd_dir + ' '
                if True:
                    if chip == 'best2001':
                        cmd_list += pri_dir + ' '
                    if chip == 'best2003':
                        cmd_list += '--otp' + ' '
                        cmd_list += pri_dir + ' '
                cmd_list += release_file_dir + '/' + sign_file_name
                print('Start sign file...')
                if log_switch:
                    print(cmd_list,'\n')
                result = os.popen(cmd_list)
                output_terminal = result.read()
                list_line = ''
                for line in output_terminal.split('\n'):
                    if len(line) != 0:
                        print(line)
                        list_line = line
                if list_line != 'Code signature updated':
                    raise RuntimeError(list_line + ". %s sign file failed.\n"%(sign_file_name))
                print("%s sign file done.\n"%(sign_file_name))


# sign release_bin/*.bin --- according to configuration(bes_sign_cfg.yaml)
if True:
    # sign bin in list SIGN_FILE_LIST
    print('\n','Start signing bin in list SIGN_FILE_LIST...')
    dict_sign_file = cfg_dict['SIGN_FILE_LIST']
    sign_bin = bin_sign_fun(dict_sign_file)


