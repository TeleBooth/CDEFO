module Jekyll
  class WithinCategoryPostNavigation < Generator
    def generate(site)
      site.categories.each_pair do |category, pages|
        pages.sort! { |a,b| b <=> a}
        pages.each do |post|
          index = posts.index post
          next_in_category = nil
          previous_in_category = nil
          if index
            if index < pages.length - 1
              next_in_category = pages[index + 1]
            end
          	if index > 0
              previous_in_category = pages[index - 1]
            end
          end
          post.data["next_in_category"] = next_in_category unless next_in_category.nil?
          post.data["previous_in_category"] = previous_in_category unless previous_in_category.nil?
        end
      end
    end
  end
end

