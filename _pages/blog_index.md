---
layout: page
title: Blog
permalink: /blog/
---

Once the summer hits, any large commits to the project will be accompanied by a subsequent blog post describing progress.
<ul>
  {% for post in site.posts %}
    <li>
<<<<<<< HEAD
      <a href="{{site.baseurl}}{{ post.url }}">{{ post.title }}</a>
=======
      <a href="{{ site.baseurl }}{{ post.url }}">{{ post.title }}</a>
>>>>>>> b774fe1b2262d5f5b69943ce7803131db02e91a8
      {{ post.excerpt }}
    </li>
  {% endfor %}
</ul>
