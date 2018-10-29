---
layout: page
title: Blog
permalink: /blog/
---

Once the summer hits, any large commits to the project will be accompanied by a subsequent blog post describing progress.
<ul>
  {% for post in site.posts %}
    <li>
      <a href="{{ site.baseurl }}{{ post.url }}">{{ post.title }}</a>
      {{ post.excerpt }}
    </li>
  {% endfor %}
</ul>