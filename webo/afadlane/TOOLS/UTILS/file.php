<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>HTML + PHP Example</title>
</head>
<body>
    <header>
        <h1>Welcome to our website!</h1>
    </header>
    <section>
        <p>This is a simple example of HTML and PHP integration.</p>
        <?php
            echo "<heade>Hello from PHP-FPM!</<heade>";
        ?>
    </section>
    <footer>
        <p>&copy; <?php echo date('Y'); ?> Our Website</p>
    </footer>
</body>
</html>
