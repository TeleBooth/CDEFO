---
layout: page
title: Build Log
permalink: /log/
---

These are the research journals that I turned in to my Capstone Advisor every week during the semester.
<ul>
	{% for file in site.static_files %}
			{% if file.extname == ".pdf" and file.basename contains "Research Journal"%}
				<li>
					<a href="{{site.baseurl}}{{ file.path }}"> {{file.basename}} </a>
				</li>
			{% endif %}
	{% endfor %}
</ul>