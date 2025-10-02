FROM alpine:3.20

RUN apk update && apk add --no-cache \
    git \
    nodejs \
    npm \
    gcc

RUN npm install -g @google/gemini-cli