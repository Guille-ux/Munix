#!/usr/bin/python3

import json
import os
import subprocess

def get_flat_list_from_a_dir(directory):
    flat_list=[]
    for dirpath, dirnames, filenames in os.walk(directory):
        for filename in filenames:
            filepath=os.path.join(dirpath, filename)
            flat_list.append((filepath, filename))
    return flat_list
print("[+] Pybuild [+]")
try:
    with open("pybuild.json") as f:
        data=json.load(f)
	
    c_compiler=data["cpath"]
    c_flags=data["cflags"]
    build_dir=data["build-dir"]
    source_dir=data["source-dir"]

    if not os.path.exists(build_dir):
    	os.makedirs(build_dir)
    files = get_flat_list_from_a_dir(source_dir)
    for filepath, file in files:
        if file.endswith(".c"):
            obj_name = os.path.splitext(file)[0] + ".o"
            obj_path = os.path.join(build_dir, obj_name)
            print(f"[+] Compiling {file} -> {obj_path} ... [+]")
            result=subprocess.run(
		        [c_compiler] + c_flags.split() + ["-c", filepath, "-o", obj_path],
		    capture_output=True,			    text=True
		    )
		
            if result.returncode != 0:
                print(f"[!] Error Compiling {file} [!]")
                print(result.stderr)
            else:
                print(f"[+] {file} Compiled Successfully [+]")
except FileNotFoundError:
	print("[+] ´pybuild.json´ not found [+]")
