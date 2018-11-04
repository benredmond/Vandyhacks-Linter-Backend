module.exports = {

    parse: function (lineToDescription, typeToCount, errors){
        //perform some formatting - remove blank line at end and split error block
        //into an array of errors
        var trimmedErrors = errors.substr(0, errors.length - 1);
        var lines = trimmedErrors.split("\n");
        //iterate through each error
        var errorNum = 0;
        while (errorNum !== lines.length){
            //keep track of the indexes of the colons
            var index = 0;
            var colonCount = 0;
            var firstColonIndex;
            var secondColonIndex;
            var thirdColonIndex;
            //iterate through characters in an error
            while (index <= lines[errorNum].length){
                if ((lines[errorNum])[index] === ':'){
                    colonCount++;
                    if (colonCount === 1){
                        firstColonIndex = index;
                    }
                    else if (colonCount === 2){
                        secondColonIndex = index;
                    }
                    else if (colonCount === 3){
                        thirdColonIndex = index;
                    }
                }
                index++;
            }
            //let's map these colon-blocks
            if (colonCount !== 0) {
                var lineNum = lines[errorNum].substr(firstColonIndex + 1, (secondColonIndex - (firstColonIndex + 1)));
                var messageType = lines[errorNum].substr(secondColonIndex + 2, (thirdColonIndex - (secondColonIndex + 2)));
                var message = lines[errorNum].substr(thirdColonIndex + 2, lines[errorNum].length - 1);
                lineToDescription[lineNum - 2] = message;
                typeToCount[messageType] = typeToCount[messageType] + 1;

            }errorNum++;
        }

    }
}
