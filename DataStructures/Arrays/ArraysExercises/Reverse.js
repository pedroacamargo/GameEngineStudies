// Create a function that reverses a string:
// 'Hi My name is Andrei' should be:
// 'ierdnA si eman yM iH'

const reverse = (str) => {

    if (!str || str.length < 2 || typeof str !== 'string') {
        return 'hmm that is not good';
    }

    let step = str.length;
    for (let i = 0; i < str.length/2; i++) {
        const aux = str[i];
        str[i] = str[step];
        str[step] = aux;
        step--;
    }
    return str;
}

const reverse2 = str => [...str].reverse().join('')

console.log(reverse2('Hi My name is Andrei'));