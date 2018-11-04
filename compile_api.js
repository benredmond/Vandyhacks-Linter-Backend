const request = require('request');

module.exports = function (code, callback) {
    let json = {
        "source_code": code,
        "language": "cpp",
        "api_key": "guest"
    };

    request({
            url: 'http://api.paiza.io/runners/create',
            method: "POST",
            json
        },
        function (error, response, body) {
            console.log('body:', body);
            setTimeout(function() {
                request.get(`http://api.paiza.io/runners/get_details?id=${body.id}&api_key=guest`, function (error, response, body) {
                    console.log(body);
                    callback(body);
                });
            }, 3000);

        });
};
