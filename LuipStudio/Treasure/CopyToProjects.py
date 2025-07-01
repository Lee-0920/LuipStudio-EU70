#!/usr/bin/env python
# -*- encoding: utf-8 -*-
'''
@File    :  CopyToProjects.py
@Date    :  2021/04/02
@Author  :  Yaronzz
@Version :  1.0
@Contact :  yaronhuang@foxmail.com
@Desc    :  
'''
import os
import aigpy

FOLDER = "Treasure"
THIS_PATH = str.replace("E:/code/work/Treasure", "\\", "/")
PROJECTS_PATH = ["E:/code/work/EM60-WS/EM60Studio/",
                 "E:/code/work/MWMS-300/MwmsStudio_main_bak/",
                 "E:/code/work/SAMS/SamsStudio/"]


FILES = aigpy.path.getFiles(THIS_PATH)
for project in PROJECTS_PATH:
    print("=======Copy to " + project + "=========")

    path = project + '/' + FOLDER + "/"
    aigpy.path.remove(path)
    aigpy.path.mkdirs(path)

    for file in FILES:
        if ".svn" in file:
            continue
        if ".user" in file:
            continue
        if ".py" in file:
            continue

        newfile = path + str.replace(file, THIS_PATH, "")
        if aigpy.path.copyFile(file, newfile) is False:
            print("Err: copy failed [" + newfile + "]")
    print("=======Copy to " + project + " finish=========")

print("Copy all finish.")
aigpy.cmd.inputInt("",0)








