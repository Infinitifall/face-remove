
/**
 * General purpose functions
 */


/**
 * Multiply two matrices
 * @param {Array 2d} a the pre multiplicative matrix
 * @param {Array 2d} b the post multiplicative matrix
 * @returns {Array 2d} result matrix
 */
function matrixMultiply(matrix_1, matrix_2) {
    let result = new Array(matrix_1.length).fill(0);
    for (let i = 0; i < result.length; i++) {
        result[i] = new Array(matrix_2[i].length).fill(0);
        for (let j = 0; j < result[i].length; j++) {
            for (let k = 0; k < matrix_1[i].length; k ++) {
                result[i][j] += matrix_1[i][k] * matrix_2[k][j];
            }
        }
    }
    return result;
}


/**
 * Rotate a 3D vector
 * @param {Array 1d} vector the 3D vector
 * @param {Array 1d} r the 3D rotation
 * @returns {Array 1d} result 3D vector
 */
function rotateVector3D(vector, r) {
    let result = [[vector[0]], [vector[1]], [vector[2]]];
    let rotationMatrixX = [
        [1, 0, 0],
        [0, Math.cos(r[0]), -Math.sin(r[0])],
        [0, Math.sin(r[0]), Math.cos(r[0])]
    ];
    let rotationMatrixY = [
        [Math.cos(r[1]), 0, Math.sin(r[1])],
        [0, 1, 0],
        [-Math.sin(r[1]), 0, Math.cos(r[1])]
    ];
    let rotationMatrixZ = [
        [Math.cos(r[2]), -Math.sin(r[2]), 0],
        [Math.sin(r[2]), Math.cos(r[2]), 0],
        [0, 0, 1]
    ];

    let temp_1 = matrixMultiply(rotationMatrixX, rotationMatrixY);
    temp_1 = matrixMultiply(temp_1, rotationMatrixZ);
    temp_1 = matrixMultiply(temp_1, result);
    return result = [temp_1[0][0], temp_1[1][0], temp_1[2][0]];
}


/**
 * Rotate a 3D vector (inverse)
 * @param {Array 1d} vector the 3D vector
 * @param {Array 1d} r the 3D rotation
 * @returns {Array 1d} result 3D vector
 */
function rotateVector3DInverse(vector, r) {
    let result = [[vector[0]], [vector[1]], [vector[2]]];
    let rotationMatrixX = [
        [1, 0, 0],
        [0, Math.cos(r[0]), Math.sin(r[0])],
        [0, -Math.sin(r[0]), Math.cos(r[0])]
    ];
    let rotationMatrixY = [
        [Math.cos(r[1]), 0, -Math.sin(r[1])],
        [0, 1, 0],
        [Math.sin(r[1]), 0, Math.cos(r[1])]
    ];
    let rotationMatrixZ = [
        [Math.cos(r[2]), Math.sin(r[2]), 0],
        [-Math.sin(r[2]), Math.cos(r[2]), 0],
        [0, 0, 1]
    ];

    let temp_1 = matrixMultiply(rotationMatrixZ, rotationMatrixY);
    temp_1 = matrixMultiply(temp_1, rotationMatrixX);
    temp_1 = matrixMultiply(temp_1, result);
    return result = [temp_1[0][0], temp_1[1][0], temp_1[2][0]];
}


/**
 * Scale a vector
 * @param {Array 1d} vector the nD vector
 * @param {Float} scale
 * @returns {Array 1d} result nD vector
 */
function scaleVector(vector, scale) {
    let result = new Array(vector.length);
    for (let i = 0; i < vector.length; i++) {
        result[i] = vector[i] * scale;
    }
    return result;
}


/**
 * Add two vectors
 * @param {Array 1d} vector_1 an nD vector
 * @param {Array 1d} vector_2 an nD vector
 * @returns {Array 1d} result nD vector
 */
function addVectors(vector_1, vector_2) {
    let result = new Array(vector_1.length);
    for (let i = 0; i < vector_1.length; i++) {
        result[i] = vector_1[i] + vector_2[i];
    }
    return result;
}


/**
 * Get vectors parallel to the sides of a rotated cube
 * @param {Array 1d} s the size of cube
 * @param {Array 1d} r the rotation of cube
 * @returns {Array 3d of Array 1d} result vectors
 */
function getSideVectors(s, r) {
    let temp_1 = rotateVector3D([s[0],0,0], r);
    let temp_2 = rotateVector3D([0,s[1],0], r);
    let temp_3 = rotateVector3D([0,0,s[2]], r);
    return [temp_1, temp_2, temp_3];
}


/**
 * Get an array of points for face
 * @param {Array 1d} p the position of cube
 * @param {Array 1d} s the size of cube
 * @param {Array 1d} r the rotation of cube
 * @param {Integer} face the face number of cube, between 0 and 5 both inclusive
 * @param {Integer} divisions the number of columns and rows you want per face grid
 * @returns {Array of Array 3d} result
 */
function getFacePoints(p, s, r, face, divisions) {
    let side_vectors = getSideVectors(s, r);

    let vector_1 = side_vectors[face % 3];
    let vector_2 = side_vectors[(face + 1) % 3];
    let vector_3 = side_vectors[(face + 2) % 3];
    let sign = (face % 2) ? -1 : 1;

    let offset = addVectors(scaleVector(vector_1, -0.5), scaleVector(vector_2, -0.5));
    offset = addVectors(offset, scaleVector(vector_3, 0.5 * sign));
    offset = addVectors(offset, p);
    let vector_1_div = scaleVector(vector_1, 1 /divisions);
    let vector_2_div = scaleVector(vector_2, 1 / divisions);

    let points = new Array();
    for (let i = 0; i <= divisions; i ++) {
        for (let j = 0; j <= divisions; j ++) {
            let temp_vector = addVectors(scaleVector(vector_1_div, i), scaleVector(vector_2_div, j));
            temp_vector = addVectors(temp_vector, offset);
            points.push(temp_vector);
        }   
    }
    return points;
}


/**
 * Check is a point is inside a cube
 * @param {Array 1d} point 3d vector 
 * @param {Array 1d} p the position of cube
 * @param {Array 1d} s the size of cube
 * @param {Array 1d} r the rotation of cube
 * @param {Float} error the amount of error you are willing to accept (0 for exact check)
 * @returns {Boolean} true if inside else false
 */
function checkPointInsideCube(point, p, s, r, error) {
    let new_point = rotateVector3DInverse(point, r);
    let new_p = rotateVector3DInverse(p, r);
    new_point = addVectors(new_point, scaleVector(new_p, -1));
    if (
        (-s[0]/2 <= (new_point[0] + error)) && ((new_point[0] - error) <= s[0]/2) &&
        (-s[1]/2 <= (new_point[1] + error)) && ((new_point[1] - error) <= s[1]/2) &&
        (-s[2]/2 <= (new_point[2] + error)) && ((new_point[2] - error) <= s[2]/2)
    ) {
        return true;
    } else {
        return false;
    }
}


/**
 * Housekeeping functions
 */


/**
 * Gets the real p vector instead of the fake one
 * @param {Array 1d} p the position of cube (fake)
 * @param {Array 1d} s the size of cube
 * @param {Array 1d} r the rotation of cube
 * @param {Array 1d} p the position of cube
 * @returns {Array 1d} the position of cube (real)
 */
function getRealP(p,s,r) {
    return addVectors(p, rotateVector3D([0, s[1] / 2,0], r));
}


function getRealRProperty(map_json) {
    for (let i = 0; i < map_json.objects.length; i++) {
        if (!('r' in map_json.objects[i])) {
            map_json.objects[i]['r'] = [0,0,0];
        }
    }
    return map_json;
}


/**
 * Gets the real s property
 * @param {JSON} map_json
 * @returns {JSON} map_json
 */
function getRealSProperty(map_json) {
    for (let i = 0; i < map_json.objects.length; i++) {
        if ('si' in map_json.objects[i]) {
            map_json.objects[i]['s'] = [
                map_json.xyz[map_json.objects[i]['si'] * 3 + 0],
                map_json.xyz[map_json.objects[i]['si'] * 3 + 1],
                map_json.xyz[map_json.objects[i]['si'] * 3 + 2]
            ];
            delete map_json.objects[i]['si'];
        }
    }
    return map_json;
}


/**
 * Gets the fake face_index from the real one
 * @param {Integer} face_index (real) between 0 and 5 both inclusive
 * @returns {Integer} the face index (fake) between 0 and 5 both inclusive
 */
function getFakeFaceIndex(face_index) {
    switch(face_index) {
        case 0:
            return 4;
        case 1:
            return 1;
        case 2:
            return 2;
        case 3:
            return 5;
        case 4:
            return 0;
        case 5:
            return 3;
    }
}


/**
 * Remove faces given a map_export
 * @param {JSON} map_export
 * @returns {JSON} map_export
 */
function removeFaces(map_export) 
{
    let map_json = JSON.parse(map_export);
    map_json = getRealRProperty(map_json);
    map_json = getRealSProperty(map_json);    
    let map_objects = map_json.objects;
    let object_count = map_objects.length;
    let face_count_old = 0;
    let face_count_new = 0;
    for(let i = 0; i < object_count ; i++) {
        if ('i' in map_objects[i]) {
            continue;
        }
        console.log('Processing object ' + String(i));
        map_json.objects[i]['h'] = i;
        for(let face = 0; face < 6 ; face++) {
            let face_fake = getFakeFaceIndex(face);
            if ('f' in map_objects[i]) {
                let old_face = parseInt(map_objects[i]['f'], 16);
                if (old_face.toString(2).charAt(face_fake) == "0") {
                    // do nothing, 0 -> already deactivated
                    continue;
                }
            }
            let real_p = getRealP(map_objects[i]['p'], map_objects[i]['s'], map_objects[i]['r']);
            let face_points = getFacePoints(real_p, map_objects[i]['s'], map_objects[i]['r'], face, 10);
            let covered = true;
            for (let j = 0; j < face_points.length; j++) {
                let covered_once = false;
                for(let k = 0; k < object_count ; k++) {
                    if (i == k) {
                        continue;
                    }
                    if ('i' in map_objects[k]) {
                        continue;
                    }
                    let real_p_2 = getRealP(map_objects[k]['p'], map_objects[k]['s'], map_objects[k]['r']);
                    if (checkPointInsideCube(face_points[j], real_p_2, map_objects[k]['s'], map_objects[k]['r'], 0.1)) {
                        covered_once = true;
                        // map_json.objects.push({"p":face_points[j],"s":[0.3,0.3,0.3],"c":"#ff6600","h":k})
                        break;
                    } else {
                        // map_json.objects.push({"p":face_points[j],"s":[0.3,0.3,0.3],"c":"#0066ff"})
                    }
                }
                if (covered_once == false) {
                    covered = false;
                    break;
                }
            }
            if (covered) {
                if ('f' in map_objects[i]) {
                    let old_face = parseInt(map_objects[i]['f'], 16);
                    if (old_face.toString(2).charAt(face_fake) == "0") {
                        // do nothing, 0 -> already deactivated
                    } else {
                        face_count_old += 1;
                        old_face -= Math.pow(2, 5 - face_fake);
                        map_json.objects[i]['f'] = old_face.toString(16);
                    }
                } else {
                    face_count_old += 1;
                    map_json.objects[i]['f'] = (Math.pow(2, 6) - 1 - Math.pow(2, 5 - face_fake)).toString(16);
                }
            } else {
                face_count_old += 1;
                face_count_new += 1;
            }
        }
    }
    console.log("Old face count " + String(face_count_old));
    console.log("New face count " + String(face_count_new));
    console.log("Reduction " + (100*(face_count_old - face_count_new)/ face_count_old).toFixed(2) + "%");
    return map_json;
}


/**
 * The main function
 * @returns 
 */
function main() {
    try {
        let map_export = document.getElementById('input').value;
        map_export = removeFaces(map_export);
        map_export = JSON.stringify(map_export)
        let output = document.getElementById('output');
        output.value = map_export;
    } catch (e) {
        alert("Error: " + e);
    }
}

