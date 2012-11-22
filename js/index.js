(function() {
    var numPoints = 100;
    var points = [];
    var pointsToRender = [];
    var time = 0;

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
                ctx.fillStyle = 'rgb(' + Math.round(r + dr * (numPoints - i)) + ','
                                       + Math.round(g + dg * (numPoints - i)) + ','
                                       + Math.round(b + db * (numPoints - i)) + ')';
                ctx.fillRect(pointsToRender[i].x, height - pointsToRender[i].y, 3, 3);
            }
        }

        $("#record").click(function() {
            recording = true;
            console.log("Recording")
            points = [];
        });
        $("#stop").click(function() {
            recording = false;
            printPoints(points);
        });

        canvas.addEventListener('mousedown', handleEvent, false);
        canvas.addEventListener('mousemove', handleEvent, false);
        canvas.addEventListener('mouseup',   handleEvent, false);
    });

    var printPoints = function(points) {
        var str = "x<-c(";
        var delimiter = "";
        for (var i = 0; i < points.length;i++) {
            str = str + delimiter + points[i].x;
            delimiter = ",";
        }
        console.log(str+")")
        var str = "y<-c(";
        var delimiter = "";
        for (var i = 0; i < points.length;i++) {
            str = str + delimiter + points[i].y;
            delimiter = ",";
        }
        console.log(str+")")

    }
})();
