<?php

header("Content-Type: application/json");

include "db.php";

$sql = "SELECT * FROM sensor_data
        ORDER BY created_at DESC
        LIMIT 50";

$result = $conn->query($sql);

$data = [];

while ($row = $result->fetch_assoc()) {
    $data[] = $row;
}

echo json_encode($data);

$conn->close();

?>