FROM debian:11

RUN apt update -y

WORKDIR /

RUN apt install -y nginx 
RUN apt install -y nano


COPY afadlane .
# RUN sudo chown -R www-data:www-data /afadlane

COPY ./ngnix.conf /etc/nginx/sites-enabled/default.conf 

CMD [ "nginx", "-g", "daemon off;" ]