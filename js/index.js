(function() {
    var numPoints = 300;
    var points = [];
    var pointsToRender = [];
    var time = 0;
    var recordings = {};

    var width = {
        canvas : 500,
        preview : 250
    }

    var height = {
        canvas : 500,
        preview : 250
    }

    var name;

    var Point = function(x, y) {
        this.x = x;
        this.y = y;
    }
    var recording = false;

    $(document).ready(function() {
        name = prompt("Enter your name (lowercase, no spaces):");
        //name = 'sharad';
        var canvas = {
            "canvas" : document.getElementById('canvas'),
            "preview" : document.getElementById('preview')
        };
        var context = {};
        for (var i in canvas) {
            if (canvas[i].getContext) {
                var ctx = canvas[i].getContext("2d");
                context[i] = ctx;
            }
        }

        var handleEvent = function(e) {
            var x,y;

            if(e.offsetX) {
                x = e.offsetX;
                y = e.offsetY;
            } else if(e.layerX) {
                x = e.layerX;
                y = e.layerY;
            }
            addPoint(x,height['canvas'] - y);
        }
	leap.on('frame', function(id, data) {
 	    var hands = data.hands;
	    if (hands[0].fingers[0]) {
		var point = hands[0].fingers[0].tip.position.slice(0,2);
		point = { x : point[0]+250, y : point[1]+100 };
		addPoint(point.x, point.y);
	    }
	});

        var addPoint = function(x,y) {
            var point = new Point(x,y);
            if (recording)
                points.push(point);
            pointsToRender.push(point);
            if (pointsToRender.length > numPoints) {
                pointsToRender.shift()
            }
            time++;
            render('canvas', true, pointsToRender);
        }

        var render = function(cv, fade, ptr) {
            if (fade === undefined) {
                fade = true;
            }
            context[cv].clearRect(0,0,width[cv],height[cv]);
            var r = 255;
            var g = 0;
            var b = 0;
            var length = ptr.length;
            if (fade) {
                var dr = (255 - r)/length;
                var dg = (255 - g)/length;
                var db = (255 - b)/length;
            } else {
                var dr = 0
                var dg = 0
                var db = 0
            }

            for (var i = 0; i < ptr.length; i++) {
                var point = { x : Math.floor(ptr[i].x / width['canvas'] * width[cv]), y : Math.floor(ptr[i].y / height['canvas'] * height[cv]) }
                context[cv].fillStyle = 'rgb(' + Math.round(r + dr * (length - i)) + ',' + Math.round(g + dg * (length - i)) + ',' + Math.round(b + db * (length - i)) + ')';
                context[cv].fillRect(Math.floor(point.x), Math.floor(height[cv] - point.y), 3, 3);
            }
        }

        $("#record").click(function() {
	    if (!recording) {
		recording = true;
		console.log("Recording")
		points = [];
		$("#display").text("Recording");
		$("#record").text("Stop");
	    } else {
		recording = false;
		$("#display").text("Points recorded: "+points.length);
	        render("preview", false, points);
		var char = prompt("What character did you record? (case sensitive)");
		if (char) {
		    if (!recordings[char]) {
		       recordings[char] = [];
		    }
		    render("preview", false, points);
		    recordings.timestamp = new Date().getTime();
		    recordings[char].push(points);
		}
		$("#record").text("Record");
	    }
        });
        $("#stop").click(function() {
        });
        $("#upload").click(function() {
            console.log(recordings)
            $.post("/upload", JSON.stringify( { document : { "_id" : name+"-"+recordings.timestamp, recordings :recordings}})).success(function () {
                alert("uploaded successfully!");
            });
        });

        canvas['canvas'].addEventListener('mousedown', handleEvent, false);
        canvas['canvas'].addEventListener('mousemove', handleEvent, false);
        canvas['canvas'].addEventListener('mouseup',   handleEvent, false);
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
