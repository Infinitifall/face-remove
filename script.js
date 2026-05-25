import createModule from "./build/face-remove.js";


function getRealSProperty(map_json) {
    for (let i = 0; i < map_json['objects'].length ; i++) {
      if ('si' in map_json['objects'][i]) {
        map_json['objects'][i]['s'] = [
          map_json['xyz'][map_json['objects'][i]['si'] * 3 + 0],
          map_json['xyz'][map_json['objects'][i]['si'] * 3 + 1],
          map_json['xyz'][map_json['objects'][i]['si'] * 3 + 2]
        ];
      }
    }
  return map_json;
}


function checkIfUse(map_object) {
  if ('i' in map_object) {
      return false;
  } else if ('o' in map_object) {
    if (map_object['o'] != 1) {
        return false;
    }
  } else if ('ft' in map_object) {
    if (map_object['ft'] == 1) {
        return false;
    }
  } else if ('v' in map_object) {
    if (map_object['v'] == 1) {
        return false;
    }
  } else if ('t' in map_object) {
    if ([7, 12].includes(map_object['t'])) {
        return false;
    }
  } return true;
}


function make_c_input(map_export) {
  map_export = getRealSProperty(map_export);
  let return_string = '';
  let map_objects = map_export['objects'];
  let object_count = map_objects.length;
  
  for (let i = 0; i < object_count; i++) {
    if (!checkIfUse(map_objects[i])) {
      continue;
    }
    
    if ('f' in map_objects[i]) {
      let new_f = parseInt(map_objects[i]['f'], 16).toString(2);
      return_string += '0'.repeat(6 - new_f.length) + `${new_f}\n`;
    } else {
      return_string += '111111\n';
    }
    
    if ('p' in map_objects[i]) {
      for (let j = 0; j < 3; j++) {
        return_string += `${map_objects[i]["p"][j]}\n`;
      }
    } else {
      return_string += '0\n0\n0\n';
    }
    
    if ('s' in map_objects[i]) {
      for (let j = 0; j < 3; j++) {
        return_string += `${map_objects[i]["s"][j]}\n`;
      }
    } else {
      return_string += '10\n10\n10\n';
    }
    

    if ('r' in map_objects[i]) {
      for (let j = 0; j < 3; j++) {
        return_string += `${map_objects[i]["r"][j]}\n`;
      }
    } else {
        return_string += '0\n0\n0\n';
    }
  }

  return return_string;
}



function make_c_output(map_export, c_output) {
    map_export = getRealSProperty(map_export);
    c_output = c_output.split('\n');
    let real_i = 0;
    for (let i = 0; i < map_export['objects'].length; i++) {
      if (!checkIfUse(map_export['objects'][i])) {
        continue;
      }

      map_export['objects'][i]['f'] = (parseInt(c_output[real_i], 2)).toString(16);
      real_i += 1;
    }
    
    return map_export;
}


let ui = {
  input:          document.getElementById("input"),
  optimize_faces: document.getElementById("optimize_faces"),
  output:         document.getElementById("output"),
  copy_output:    document.getElementById("copy_output"),
  output_log:     document.getElementById("output_log"),
};


let console_log = "";


createModule({
  // override defaults
  print: (text) => {
    // console.log(text);
    console_log += text + '\n';
  },
  printErr: (text) => {
    // console.error(text);
    console_log += text + '\n';
  },

}).then((Module) => {
  console.log("Wasm ready", Module);

  let api = {
    test_wasm:          Module.cwrap("test_wasm", "number", []),
    removeFacesWrapper: Module.cwrap("removeFacesWrapper", "number", ["number"]),
  }

  // wasm test
  // ui.output_log.value = "test_wasm() == " + api.test_wasm();

  // face-remove
  let face_remove = (input_text) => {
    let json_input = JSON.parse(input_text);
    let c_input = make_c_input(json_input);

    // call c program
    const input_ptr = Module.stringToNewUTF8(c_input);  // malloc, copy, null terminate; much better than doing it manually via HEAP and get/set!
    const output_ptr = api.removeFacesWrapper(input_ptr);
    const c_output = Module.UTF8ToString(output_ptr);  // will scan till null terminator; much better than manually scanning via HEAP
    Module._free(input_ptr);
    Module._free(output_ptr);

    let output_json = make_c_output(json_input, c_output);
    let output_text = JSON.stringify(output_json);
    return output_text;
  };

  // bind to buttons
  ui.optimize_faces.addEventListener("click", (() => {
    console_log = "";
    ui.output.value = face_remove(ui.input.value);
    ui.output_log.value = console_log;  // this avoids DOM updates during runtime (slow!)
    ui.output_log.scrollTop = ui.output_log.scrollHeight;  // scroll to bottom
  }));
  ui.copy_output.addEventListener("click", (() => {
    navigator.clipboard.writeText(ui.output.value);
  }))
});

