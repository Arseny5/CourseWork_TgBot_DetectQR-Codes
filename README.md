# coursework
openssl req -newkey rsa:2048 -sha256 -nodes -keyout private.key -x509 -days 365 -out public.pem
cat private.key public.pem > cert.pem
curl -F"url=https://ВАШ_IP:ПОРТ(либо 443, либо 8443)/ЛЮБОЙ_URI(можно и без него, я буду использовать токен)/" -F"certificate=@public.pem" https://api.telegram.org/botТОКЕН/setWebhook/
