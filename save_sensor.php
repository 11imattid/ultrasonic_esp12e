<?php

header("Content-Type: application/json");

include "db.php";

$data = json_decode(file_get_contents("php://input"), true);

if (!$data) {

    echo json_encode([
        "status" => "error",
        "message" => "No data received"
    ]);

    exit();
}

$distance = $data["distance"];
$motion = $data["motion"];

$sql = "INSERT INTO sensor_data(distance, motion)
        VALUES ('$distance', '$motion')";

if ($conn->query($sql) === TRUE) {

    echo json_encode([
        "status" => "success",
        "message" => "Data stored successfully"
    ]);

} else {

    echo json_encode([
        "status" => "error",
        "message" => $conn->error
    ]);
}

$conn->close();

?>