---
layout: page
title: Build Log
permalink: /log/
---

Will host all of the research journals I made over the semester.
<ul>
  {% for page in site.pages %}
	{% if page.category == 'build-log' %}
	  <li><a href="{{site.baseurl}}{{ page.url }}">{{ page.title }}</a></li>
	{% endif %}   <!-- cat-match-p -->
  {% endfor %}  <!-- page -->
</ul>
