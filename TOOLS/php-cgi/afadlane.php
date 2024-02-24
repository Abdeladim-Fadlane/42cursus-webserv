<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Home Page</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f4f4f4;
            margin: 0;
            padding: 0;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
        }
        .container {
            max-width: 800px;
            padding: 20px;
            background-color: #fff;
            border-radius: 5px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
            text-align: center; /* Center align text within container */
        }
        h1 {
            color: #333;
        }
        p {
            line-height: 1.6;
            color: #666;
        }
        .logout-btn {
            display: inline-block;
            width: 100px;
            margin-top: 20px;
            padding: 10px;
            background-color: #4CAF50;
            color: #fff;
            text-decoration: none;
            border-radius: 5px;
        }
        .logout-btn:hover {
            background-color: #45a049;
        }
        .profile-image {
            width: 200px; /* Set width */
            height: 200px; /* Set height to match width */
            border-radius: 50%; /* Set border-radius to 50% for circular shape */
            margin-bottom: 20px;
        }
    </style>
</head>
<body>
    <div class="container">
        <img src="afadlane1.jpeg" alt="Abdeladim Fadlane" class="profile-image">
        <h1>Welcome to my Portfolio</h1>
        <p>Hey there! I'm Abdeladim Fadlane, a dedicated and enthusiastic developer currently navigating the exciting world of programming as a student at 1337. My passion lies in the realms of C and C++, Linux, Docker, and the intricate symphony of commands in Bash.</p>
        <p>This is a perfect home page example with enhanced styling and features.</p>
        <p>You can add more content, such as:</p>
        <ul>
            <li>Links to different sections of your website</li>
            <li>Featured products or services</li>
            <li>News or blog updates</li>
            <li>Testimonials or customer reviews</li>
            <li>Contact information or a contact form</li>
        </ul>
        <a href="logout.php" class="logout-btn">Logout</a>
    </div>
</body>
</html>
