<!DOCTYPE html>
<html>
<head>
    <title>CGI POST Test</title>
</head>
<body>
    <h1>CGI POST Test</h1>

    <?php
    // Check if POST data is available
    if ($_SERVER['REQUEST_METHOD'] === 'POST') {
        // Print received POST data
        echo "<p>Received POST data:</p>\n";
        echo "<pre>" . print_r($_POST, true) . "</pre>\n";
    } else {
        // No POST data available
        echo "<p>No POST data received.</p>\n";
    }
    ?>

    <form action="" method="post">
        <label for="name">Name:</label>
        <input type="text" id="name" name="name"><br><br>
        <label for="email">Email:</label>
        <input type="email" id="email" name="email"><br><br>
        <input type="submit" value="Submit">
    </form>
</body>
</html>
