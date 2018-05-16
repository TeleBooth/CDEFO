---
layout: page
title: Tutorials
permalink: /tut/
---

General Tutorial on setting up the software environment. A bonus caveat about using NeoPixels.
<ul>
  {% for page in site.pages %}
	{% if page.category == 'tutorial' %}
	  <li><a href="{{site.baseurl}}{{ page.url }}">{{ page.title }}</a></li>
	{% endif %}   <!-- cat-match-p -->
  {% endfor %}  <!-- page -->
</ul>