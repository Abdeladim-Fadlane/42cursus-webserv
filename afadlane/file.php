<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>PHP CGI Output</title>
</head>
<body>
    <h1>PHP CGI Output</h1>
    <form method="post" action="<?php echo $_SERVER['PHP_SELF']; ?>">
        <label for="input">Enter your input:</label><br>
        <input type="text" id="input" name="input"><br>
        <input type="submit" value="Submit">
    </form>
    <hr>

    <?php
    if ($_SERVER["REQUEST_METHOD"] == "POST") {
        // Get user input
        $input = $_POST["input"];

        // Call CGI script
        $output = shell_exec("/path/to/your/cgi_script.cgi " . escapeshellarg($input));

        // Display output
        echo "<h2>Output:</h2>";
        echo "<pre>" . htmlspecialchars($output) . "</pre>";
    }
    ?>
</body>
</html>
