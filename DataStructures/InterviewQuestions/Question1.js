// Given 2 arrays, create a function that let's a user know (true/false) whether these two arrays contains any common items
// For example: 
// const array1 = ['a', 'b', 'c', 'x']
// const array2 = ['z', 'y', 'i']
// should return false
// --------------
// const array1 = ['a', 'b', 'c', 'x']
// const array2 = ['z', 'y', 'x']
// should return false

// 2 parameters - arrays - no size limit
// return true or false

// O(n^2) - Quadratic
function containsCommonItem(arr1, arr2) {
    for (let i = 0; i < arr1.length; i++) {
        for (let j = 0; j < arr2.length; j++) {
            if (arr1[i] == arr2[j]) return true;
        }        
    }
    return false;
}


console.log(containsCommonItem(['a', 'b', 'c', 'x'], ['z', 'y', 'x']));


/**
 * array1 => obj 
 * {
 *  a: true,
 *  b: true,
 *  c: true,
 *  x: true
 * }
 * 
 * array2[index] === obj.properties
 */
// Using HashTables
// O(n) - Linear
function containsCommonItem2(arr1, arr2) {
    // Can we assume always 2 parameters?

    // Loop through first array and create object where properties === items in the array
    let map = {};
    for (let i = 0; i < arr1.length; i++) {
        if (!map[arr1[i]]) {
            const item = arr1[i];
            map[item] = true;
        }
    }


    // Loop through second array and check if second array exists on created object
    for (let j = 0; j < arr2.length; j++) {
        if (map[arr2[j]]) {
            return true;
        }
    }

    return false;
}

console.log(containsCommonItem2(['a', 'b', 'c', 'x'], ['z', 'y', 'x']));

function containsCommonItem3(arr1, arr2) {
    return arr1.some(item => arr2.includes(item));
}
