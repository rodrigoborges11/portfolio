var deadline = new Date("Jul 17, 2024 23:59:59").getTime();
var x = setInterval(function() {
var now = new Date().getTime();
var t = deadline - now;
var days = Math.floor(t / (1000 * 60 * 60 * 24));
var hours = Math.floor((t%(1000 * 60 * 60 * 24))/(1000 * 60 * 60));
var minutes = Math.floor((t % (1000 * 60 * 60)) / (1000 * 60));
var seconds = Math.floor((t % (1000 * 60)) / 1000);

if(days<0){
    document.querySelector(".demo-days-p").innerHTML = 0;
    document.querySelector(".demo-hours-p").innerHTML = 0;
    document.querySelector(".demo-minutes-p").innerHTML = 0;
    document.querySelector(".demo-seconds-p").innerHTML = 0;

    document.querySelector(".demo-days-p2").innerHTML = 0;
    document.querySelector(".demo-hours-p2").innerHTML = 0;
    document.querySelector(".demo-minutes-p2").innerHTML = 0;
    document.querySelector(".demo-seconds-p2").innerHTML = 0;
}
else {
    document.querySelector(".demo-days-p").innerHTML = days;
    document.querySelector(".demo-hours-p").innerHTML = hours;
    document.querySelector(".demo-minutes-p").innerHTML = minutes;
    document.querySelector(".demo-seconds-p").innerHTML = seconds;
    
    document.querySelector(".demo-days-p2").innerHTML = days;
    document.querySelector(".demo-hours-p2").innerHTML = hours;
    document.querySelector(".demo-minutes-p2").innerHTML = minutes;
    document.querySelector(".demo-seconds-p2").innerHTML = seconds;
}

if (t < 0) {
    clearInterval(x);
    document.querySelector(".demo").innerHTML = "EXPIRED";
    }
}, 1000);