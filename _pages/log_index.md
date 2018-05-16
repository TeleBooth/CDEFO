---
layout: page
title: Build Log
permalink: /CDEFO/log/
---
Hi there, just testing to see if this works
<ul>
  {% for page in site.pages %}
	{% if page.category == 'build-log' %}
	  <li><a href="{{ page.url }}">{{ page.title }}</a></li>
	{% endif %}   <!-- cat-match-p -->
  {% endfor %}  <!-- page -->
</ul>
