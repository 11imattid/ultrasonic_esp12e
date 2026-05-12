<!DOCTYPE html>
<html>

<head>

    <title>Ultrasonic Sensor Dashboard</title>

    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>

    <style>

        body{
            font-family: Arial;
            background:#f2f2f2;
            padding:20px;
        }

        .card{
            background:white;
            padding:20px;
            border-radius:10px;
            margin-bottom:20px;
            box-shadow:0px 0px 10px rgba(0,0,0,0.1);
        }

        h1{
            color:#333;
        }

        .status{
            font-size:22px;
        }

    </style>

</head>

<body>

<h1>ESP8266 Ultrasonic Dashboard</h1>

<div class="card">

    <h2 id="distance">Distance: -- cm</h2>

    <h2 id="motion">Motion: --</h2>

</div>

<div class="card">

    <canvas id="chart"></canvas>

</div>

<script>

const ctx =
    document.getElementById("chart").getContext("2d");

const chart = new Chart(ctx, {

    type: "line",

    data: {

        labels: [],

        datasets: [{

            label: "Distance (cm)",

            data: [],

            borderColor: "blue",

            borderWidth: 2,

            fill: false

        }]
    }
});

async function fetchData(){

    const response =
        await fetch("get_sensor.php");

    const data = await response.json();

    if(data.length > 0){

        const latest = data[0];

        document.getElementById("distance").innerHTML =
            `Distance: ${latest.distance} cm`;

        document.getElementById("motion").innerHTML =
            `Motion: ${latest.motion}`;

        chart.data.labels =
            data.map((item, index) => index);

        chart.data.datasets[0].data =
            data.map(item => item.distance);

        chart.update();
    }
}

setInterval(fetchData, 3000);

</script>

</body>

</html>