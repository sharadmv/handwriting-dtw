(function() {
    var numPoints = 100;
    var points = [];
    var pointsToRender = [];
    var time = 0;
    var recordings = {};

    var width = 500;
    var height = 500;

    var Point = function(x, y) {
        this.x = x;
        this.y = y;
    }
    var recording = false;

    $(document).ready(function() {
        var canvas = document.getElementById('canvas');
        if (canvas.getContext) {
            var ctx = canvas.getContext("2d");
        }

        var handleEvent = function(e) {
            var x = e.clientX, y = e.clientY;
            addPoint(x,height - y);
        }

        var addPoint = function(x,y) {
            var point = new Point(x,y);
            if (recording)
                points.push(point);
            pointsToRender.push(point);
            if (pointsToRender.length > numPoints) {
                pointsToRender.shift()
            }
            time++;
            render();
        }

        var render = function() {
            ctx.clearRect(0,0,500,500);
            var r = 255;
            var g = 0;
            var b = 0;
            var dr = (255 - r)/numPoints;
            var dg = (255 - g)/numPoints;
            var db = (255 - b)/numPoints;

            for (var i = 0; i < pointsToRender.length; i++) {
                ctx.fillStyle = 'rgb(' + Math.round(r + dr * (numPoints - i)) + ',' + Math.round(g + dg * (numPoints - i)) + ',' + Math.round(b + db * (numPoints - i)) + ')';
                ctx.fillRect(pointsToRender[i].x, height - pointsToRender[i].y, 3, 3);
            }
        }

        $("#record").click(function() {
            recording = true;
            console.log("Recording")
            points = [];
            $("#display").text("Recording");
        });
        $("#stop").click(function() {
            recording = false;
            $("#display").text("Points recorded: "+points.length);
            var char = prompt("What character did you record? (case sensitive)");
            if (!recordings[char]) {
                recordings[char] = [];
            }
            recordings[char].push({ timestamp : new Date().getTime(), points : points});
            console.log(recordings);
        });
        $("#upload").click(function() {
            for (var char in recordings) {
                (function(char) {
                    for (var i in recordings[char]) {
                        printPoints(recordings[char][i].points)
                    }
                    $.ajax({
                        type: 'POST',
                        url: 'https://api.mongohq.com/databases/Handwriting/collections/documents?_apiKey=46hlx1ku4j6kg0ndg8j2',
                        data: recordings[char],
                        dataType: 'json'
                    });
                })(char);
            }
        });

        canvas.addEventListener('mousedown', handleEvent, false);
        canvas.addEventListener('mousemove', handleEvent, false);
        canvas.addEventListener('mouseup',   handleEvent, false);
    });

    var printPoints = function(points) {
        var xstr = "x<-c(";
        var delimiter = "";
        for (var i = 0; i < points.length;i++) {
            xstr = xstr + delimiter + points[i].x;
            delimiter = ",";
        }
        xstr += ")"
        var ystr = "y<-c(";
        var delimiter = "";
        for (var i = 0; i < points.length;i++) {
            ystr = ystr + delimiter + points[i].y;
            delimiter = ",";
        }
        ystr += ")"
        console.log(xstr+"\n"+ystr)

    }
})();
