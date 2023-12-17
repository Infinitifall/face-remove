import json
import subprocess
import os


def getRealSProperty(map_json: dict):
    for i in range(len(map_json['objects'])):
        if 'si' in map_json['objects'][i]:
            map_json['objects'][i]['s'] = [
                map_json['xyz'][map_json['objects'][i]['si'] * 3 + 0],
                map_json['xyz'][map_json['objects'][i]['si'] * 3 + 1],
                map_json['xyz'][map_json['objects'][i]['si'] * 3 + 2]
            ];
    return map_json;


def checkIfUse(map_object: dict):
    if 'i' in map_object:
        return False
    elif 'o' in map_object:
        if map_object['o'] != 1:
            return False
    elif 'ft' in map_object:
        if map_object['ft'] == 1:
            return False
    elif 'v' in map_object:
        if map_object['v'] == 1:
            return False
    elif 't' in map_object:
        if map_object['t'] in [7, 12]:
            return False
    return True


def make_c_input(map_export: dict):
    map_export = getRealSProperty(map_export)
    return_string = '';  
    map_objects = map_export['objects'];
    object_count = len(map_objects);
    
    for i in range(object_count):
        if not checkIfUse(map_objects[i]):
            continue;
        
        if 'f' in map_objects[i]:
            new_f = bin(int(map_objects[i]['f'], 16))[2:];
            return_string += '0' * (6 - len(new_f)) + f'{new_f}\n';
        else:
            return_string += '111111\n'
        
        if 'p' in map_objects[i]:
            for j in range(3):
                return_string += f'{map_objects[i]["p"][j]}\n'
        else:
            return_string += '0\n0\n0\n'
        
        if 's' in map_objects[i]:
            for j in range(3):
                return_string += f'{map_objects[i]["s"][j]}\n'
        else:
            return_string += '10\n10\n10\n'
        

        if 'r' in map_objects[i]:
            for j in range(3):
                return_string += f'{map_objects[i]["r"][j]}\n'
        else:
            return_string += '0\n0\n0\n'

    return return_string;



def make_c_output(map_export: dict, c_output: str):
    map_export = getRealSProperty(map_export)
    c_output = c_output.split('\n');
    real_i = 0
    for i in range(len(map_export['objects'])):
        if not checkIfUse(map_export['objects'][i]):
            continue
        
        map_export['objects'][i]['f'] = hex(int(c_output[real_i], 2))[2:]
        real_i += 1;
    
    return map_export;



if __name__ == '__main__':
    # convert input.json to input.txt for x program
    json_input = json.load(open('input.json', 'r'))
    c_input = make_c_input(json_input);
    open('input.txt', 'w').write(c_input)

    # call c program
    print('Starting face-remove\n')
    subprocess.call(["./face-remove"])

    # convert output.txt from c program to output.json
    c_output = open('output.txt', 'r').read()
    json_output = make_c_output(json_input, c_output);
    json.dump(json_output, open('output.json', 'w'))

    # remove temp files
    os.remove("input.txt")
    os.remove("output.txt")

    print('\nOutput is in output.json :)')

