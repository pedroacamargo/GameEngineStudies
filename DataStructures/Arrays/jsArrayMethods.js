const strings = ['a', 'b', 'c', 'd']
// 4 x 4 = 16 bytes

// push
strings.push('e'); // O(1)

// pop
strings.pop(); // O(1)

// unshift
strings.unshift('x'); // O(n) - It needs to loop through the whole array because it needs to reassign indexes

// splice
strings.splice(2, 0, 'alien');  // O(n)


// ARRAYS BigO's
// lookup -> O(1)
// push -> O(1)
// insert -> O(n)
// delete -> O(n)