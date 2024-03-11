
<?php

// Check if file was uploaded
if ($_FILES['file']['error'] === UPLOAD_ERR_OK) {
    // Move uploaded file to desired location
    $uploadDir = '/path/to/upload/directory/';
    $uploadFile = $uploadDir . basename($_FILES['file']['name']);

    if (move_uploaded_file($_FILES['file']['tmp_name'], $uploadFile)) {
        echo "File uploaded successfully!";
    } else {
        echo "Error uploading file.";
    }
} else {
    echo "No file uploaded.";
}

?>
