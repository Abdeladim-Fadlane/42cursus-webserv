FROM debian:11

RUN apt update -y

WORKDIR /

RUN apt install -y nginx 

COPY tools .
# COPY profile-image.jpg .

COPY ./ngnix.conf /etc/nginx/sites-enabled/default.conf 

CMD [ "nginx", "-g", "daemon off;" ]