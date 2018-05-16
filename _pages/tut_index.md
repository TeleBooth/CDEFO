---
layout: page
title: Tutorials
permalink: /CDEFO/tut/
---
Hi there, just testing to see if this works
<ul>
  {% for page in site.pages %}
	{% if page.category == 'tutorial' %}
	  <li><a href="{{ page.url }}">{{ page.title }}</a></li>
	{% endif %}   <!-- cat-match-p -->
  {% endfor %}  <!-- page -->
</ul>