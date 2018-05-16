---
layout: page
title: Documentation
permalink: /doc/
---
Hi there, just testing to see if this works
<ul>
  {% for page in site.pages %}
	{% if page.category == 'documentation' %}
	  <li><a href="{{ page.url }}">{{ page.title }}</a></li>
	{% endif %}   <!-- cat-match-p -->
  {% endfor %}  <!-- page -->
</ul>
