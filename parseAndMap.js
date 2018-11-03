module.exports = {

    parse: function (lineToDescription, typeToCount, line){
        var firstColonIndex;
        var secondColonIndex;
        var thirdColonIndex;
        var colonCount = 0;
        var index = 0;
        while (index != line.length){
            if (line.charAt(index) == ':'){
                colonCount++;
                if (colonCount == 1){
                    firstColonIndex = index;
                }
                else if (colonCount == 2){
                    secondColonIndex = index;
                }
                else if (colonCount == 3){
                    thirdColonIndex = index;
                }
            }
            index++;
        }
        var lineNum = line.substr(firstColonIndex + 1, (secondColonIndex - (firstColonIndex + 1)));
        var messageType = line.substr(secondColonIndex + 2, (thirdColonIndex - (secondColonIndex + 2)));
        var message = line.substr(thirdColonIndex + 2, line.length - 1);

        lineToDescription[lineNum] = message;
        var newCount = typeToCount[messageType];
        newCount++;
        typeToCount[messageType] = newCount;
    }
}
