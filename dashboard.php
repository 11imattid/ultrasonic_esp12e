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

        table{
            width:100%;
            border-collapse:collapse;
            margin-top:20px;
        }

        table th{
            background:#007bff;
            color:white;
            padding:12px;
            text-align:left;
        }

        table td{
            padding:10px;
            border-bottom:1px solid #ddd;
        }

        tr:nth-child(even){
            background:#f9f9f9;
        }

        .motion-yes{
            color:red;
            font-weight:bold;
        }

        .motion-no{
            color:green;
            font-weight:bold;
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

<div class="card">

    <h2>Detected Distance Records</h2>

    <table>

        <thead>

            <tr>
                <th>ID</th>
                <th>Distance (cm)</th>
                <th>Motion</th>
                <th>Date & Time</th>
            </tr>

        </thead>

        <tbody id="tableBody">

        </tbody>

    </table>

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

            fill: false,
            tension: 0.3

        }]
    }
});

async function fetchData(){

    const response =
        await fetch("get_sensor.php");

    const data = await response.json();

    if(data.length > 0){

        /*
            LATEST DATA
        */
        const latest = data[0];

        document.getElementById("distance").innerHTML =
            `Distance: ${latest.distance} cm`;

        document.getElementById("motion").innerHTML =
            `Motion: ${latest.motion == 1 ? "YES" : "NO"}`;

        /*
            UPDATE CHART
        */
        chart.data.labels =
            data.map((item, index) => index + 1);

        chart.data.datasets[0].data =
            data.map(item => item.distance);

        chart.update();

        /*
            UPDATE TABLE
        */
        let tableRows = "";

        data.forEach(item => {

            tableRows += `
                <tr>

                    <td>${item.id}</td>

                    <td>${item.distance} cm</td>

                    <td class="${
                        item.motion == 1
                        ? 'motion-yes'
                        : 'motion-no'
                    }">

                        ${
                            item.motion == 1
                            ? 'YES'
                            : 'NO'
                        }

                    </td>

                    <td>${item.created_at}</td>

                </tr>
            `;
        });

        document.getElementById("tableBody").innerHTML =
            tableRows;
    }
}

/*
    LOAD DATA EVERY 3 SECONDS
*/
setInterval(fetchData, 3000);

/*
    INITIAL LOAD
*/
fetchData();

</script>

</body>

</html>
