---
layout: page
title: Documentation
permalink: /doc/
---
Right now, it holds info about the Capstone Fair.

Soon, it will hold info about specific Python and Arduino functions calls that can be made,
and how to write your own driver sketch.

<ul>
  {% for page in site.pages %}
	{% if page.category == 'documentation' %}
	  <li><a href="{{site.baseurl}}{{ page.url }}">{{ page.title }}</a></li>
	{% endif %}   <!-- cat-match-p -->
  {% endfor %}  <!-- page -->
</ul>

This is the final proposal that I turned in at the beginning of the spring semester, and that I worked on over winter break

<a href="{{site.baseurl}}/_assets/pdfs/Booth_Final_Proposal.pdf"> Final Proposal </a>