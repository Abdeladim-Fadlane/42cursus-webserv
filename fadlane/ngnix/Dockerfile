FROM debian:11

RUN apt update -y

WORKDIR /

RUN apt install -y nginx 
RUN apt install -y nano

RUN chmod 777 -R /var/www/html
COPY afadlane .


COPY ./ngnix.conf /etc/nginx/sites-enabled/default.conf 

CMD [ "nginx", "-g", "daemon off;" ]