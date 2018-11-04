const request = require('request');

request({
        url: 'https://api.hackerearth.com/v3/code/run/',
        form: {
            'client_secret': '22212124f2f0832652e5413bbe99b6b5766029a5',
            'source': "#include <iostream>\n" +
            "int main() {\n" +
            "    std::cout << \"TEST\" << std::endl;\n" +
            "    return 0;\n" +
            "}",
            'lang': "CPP"
        },
        method: "POST"
    },
    function (error, response, body) {
        console.log('error:', error);
        console.log('statusCode:', response && response.statusCode);
        console.log('body:', body);
    });