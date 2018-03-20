# Base image containing a uwsgi server, nginx server, and Python 2.7
FROM tiangolo/uwsgi-nginx-flask:python2.7

# Install required Python packages
COPY requirements.txt /tmp
RUN pip install -r /tmp/requirements.txt
RUN rm /tmp/requirements.txt

# Copy app into container
COPY app/ /app

# Note: The container by default listens on port 80
